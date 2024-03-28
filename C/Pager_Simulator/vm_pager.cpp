#include "vm_pager.h"
#include "vm_arena.h"
// #include <unistd.h>          // this provides the actual getpid() function, and this will give the pid of the kernel, which is wrong
#include <cassert>
#include <unordered_map>
#include <vector>
#include <stdio.h>
#include <cstdlib>
#include <cstring>
typedef struct {
    void* addr;
    const char* filename;
    pid_t pid;
    unsigned int block; // Swap or Regular Block ID
    bool file_backed;   // True -> File, False -> Swap

    bool valid;
    bool resident;
    // bool readable;    // Actually Readable ? Unlike the bits in PTE
    // bool writable;    // Actually Writable ? Unlike the bits in PTE
    bool dirty;
    bool referenced;
} page_t;

typedef struct {
    std::vector<page_t> pages;          // Page meta data (referenced, dirty, valid, etc.)
    page_table_entry_t page_table[100]; // Page table with PTEs for this process
    unsigned int size;                  // SIZE of the Page table, number of PTEs
} process_t;

static unsigned int max_arena_pages;
static unsigned int max_physical_pages;
static unsigned int max_swap_pages;
static pid_t current_pid_num;   // this is used by our version of getpid()

static std::unordered_map<pid_t, process_t> processes;
static std::vector<page_t*> physical_pages;
static std::vector<bool> swap_pages;

pid_t getpid();
unsigned int addr_to_index(const void* addr);
void physical_memory_init();
void physical_pages_init();
bool physical_pages_full();
unsigned int get_physical_block();
bool swap_pages_full();
unsigned int get_swap_block();
unsigned int evict_physical_page();
void create_empty_arena(pid_t current_pid);
void make_all_zero(unsigned int ppage);
void print_virtual_page(page_t* page);
bool check_inside_arena(page_t* filename_page);
const char* get_filename_physical(page_t* page);
int make_page_resident(page_t* page);
void print_physical_pages();

/*
 * vm_init
 *
 * Called when the pager starts.  It should set up any internal data structures
 * needed by the pager.
 *
 * vm_init is passed the number of physical memory pages and the number
 * of blocks in the swap file.
 */
void vm_init(unsigned int memory_pages, unsigned int swap_blocks) {
    assert(processes.empty());
    assert(physical_pages.empty());
    assert(swap_pages.empty());

    max_physical_pages = memory_pages;
    max_swap_pages = swap_blocks;
    max_arena_pages = (uintptr_t) VM_ARENA_SIZE / VM_PAGESIZE;

    physical_pages.resize(max_physical_pages);
    physical_pages_init();

    swap_pages.reserve(max_swap_pages);

    // Pin the Zero Page in Physical Memory for all Swap-backed Pages
    physical_memory_init();
}

/*
 * vm_create
 * Called when a parent process (parent_pid) creates a new process (child_pid).
 * vm_create should cause the child's arena to have the same mappings and data
 * as the parent's arena.  If the parent process is not being managed by the
 * pager, vm_create should consider the arena to be empty.
 * Note that the new process is not run until it is switched to via vm_switch.
 * Returns 0 on success, -1 on failure.
 */
int vm_create(pid_t parent_pid, pid_t child_pid) {
    // Core: Assume Empty Arena
    create_empty_arena(child_pid);
    return 0;
}

/*
 * vm_switch
 *
 * Called when the kernel is switching to a new process, with process
 * identifier "pid".
 */
void vm_switch(pid_t pid) {
    current_pid_num = pid;
    page_table_base_register = processes[pid].page_table;
}

/*
 * vm_fault
 *
 * Called when current process has a fault at virtual address addr.  write_flag
 * is true if the access that caused the fault is a write.
 * Returns 0 on success, -1 on failure.
 */
int vm_fault(const void* addr, bool write_flag) {
    pid_t current_pid = getpid();
    // First check whether the address is valid
    if ((uintptr_t) addr < (uintptr_t) VM_ARENA_BASEADDR || 
        (uintptr_t) addr >= (uintptr_t) VM_ARENA_BASEADDR + (uintptr_t) VM_ARENA_SIZE * (processes[current_pid].size)) {
        //IARO_ADDIRION: Possible additional check from Lab 6
        // (uintptr_t) VM_ARENA_BASEADDR + (uintptr_t) VM_ARENA_SIZE * (processes[current_pid].size) < (uintptr_t) VM_ARENA_BASEADDR bc we can have adress overflow
        return -1; // Invalid Address
    }
    // Address is valid
    page_t* p = &processes[current_pid].pages[addr_to_index(addr)];
    unsigned int idx = addr_to_index(p->addr); // Calculate the PT Index from the virtual address

    print_physical_pages();

    if (processes[current_pid].page_table[idx].read_enable == false) { // Page Not Valid or Not Resident, Make resident first
        
        if (p->valid == false) {
            return -1; // Page is not Valid, but this should not happen
        }
        if (p->resident == false) { // Non-resident, make resident, mark referenced
            if (make_page_resident(p) == -1) {
                    return -1;
            }
        } else { // Resident, but Read bit is False somehow
            assert(processes[current_pid].page_table[idx].read_enable == false);
            printf("resident true but not readable");
            //throw std::runtime_error("Read Bit should have been TRUE!");
            return -1;
        }
    } // Now the Page is Resident
    if (!write_flag) { // Read Fault -> We are done
        p->referenced = true;
        return 0;
    } 
    // Write Fault -> Also need to set dirty bit in meta data and Write bit in PTE
    processes[current_pid].page_table[idx].write_enable = true;
    p->dirty = true;
    if (processes[current_pid].page_table[idx].ppage == 0) { // If writing to the "Pinned Page"
        processes[current_pid].page_table[idx].ppage = get_physical_block();
        assert(processes[current_pid].page_table[idx].ppage != 0); // Make sure page 0 is not touched
        physical_pages[processes[current_pid].page_table[idx].ppage] = p; // Reserve the page in meta data for make_all_zero() to proceed
        make_all_zero(processes[current_pid].page_table[idx].ppage); // Set the Physical Page to all zeros
        p->resident = true;
        p->referenced = true;
    }
    
    return 0;
}

/*
 * vm_destroy
 *
 * Called when current process exits.  This gives the pager a chance to
 * clean up any resources used by the process.
 */
void vm_destroy() {
    pid_t current_pid = getpid();
    // Clean up Physical Pages (meta data)
    for (size_t i=0; i<processes[current_pid].size; ++i) {
        unsigned int ppage = processes[current_pid].page_table[i].ppage;
        assert(ppage != 0); // Make sure Page 0 is not touched
        unsigned int byte = ppage * VM_PAGESIZE; // Calculate the corresponding byte
        if (physical_pages[ppage]->dirty) { // Only write back if dirty (Might be swap or file-backed page)
                if(file_write(physical_pages[ppage]->filename, physical_pages[ppage]->block, (void*)(&((char*)vm_physmem)[byte])) == -1) {
                    //throw std::runtime_error("Writing to Regular File Failed with Return Value: -1");
                    printf("Writing to Regular File Failed with Return Value: -1");
                    exit(-1);
                } // This works for both File-backed and Swap-backed Pages
        } // Now the page is written back or ignored
        physical_pages[ppage] = nullptr; // Erase Physical Page Meta Data
        // IARO addition: if we clean all the phys_data when destroying we need to write the dirty ones so copied eviction from evict phys page 
        //( I thought we would j do it when we run evict_physical_page on next process 
        // but actualy at that point the process w p_id will be erased from proccesses array so will not work)
    }

    // Clean up Swap Blocks 
    for (size_t i=0; i<processes[current_pid].pages.size(); ++i) {
        if (processes[current_pid].pages[i].file_backed == false) { // Swap-backed Page
            swap_pages[processes[current_pid].pages[i].block] = false;
        }
    }

    // Clean up Page Table
    processes[current_pid].pages.clear();
    processes.erase(current_pid);
}

/*
 * vm_map
 *
 * A request by the current process for the lowest invalid virtual page in
 * the process's arena to be declared valid.  On success, vm_map returns
 * the lowest address of the new virtual page.  vm_map returns nullptr if
 * the arena is full.
 *
 * If filename is nullptr, block is ignored, and the new virtual page is
 * backed by the swap file, is initialized to all zeroes (from the
 * application's perspective), and private (i.e., not shared with any other
 * virtual page).  In this case, vm_map returns nullptr if the swap file is
 * out of space.
 *
 * If filename is not nullptr, the new virtual page is backed by the specified
 * file at the specified block and is shared with other virtual pages that are
 * mapped to that file and block.  filename is a null-terminated C string and
 * must reside completely in the valid portion of the arena.  In this case,
 * vm_map returns nullptr if filename is not completely in the valid part of
 * the arena.
 * filename is specified relative to the pager's current working directory.
 */
void* vm_map(const char* filename, unsigned int block) {
    pid_t current_pid = getpid();

    if (processes.find(current_pid) == processes.end()) { // Process is New
        create_empty_arena(current_pid);
    } // Now Process's Arena should exist

    if (processes[current_pid].pages.size() >= max_arena_pages) { // Arena is Full
        return nullptr;
    } // Now Arena Still has spaces to allocate -> Check Physical Memory

    page_t p; // New Page Struct
    
    if (filename == nullptr) {      // Swap-backed Page, Ignore BLOCK
        if (swap_pages_full()) {    // Eager Reservation
            return nullptr;
        } // Now swap space is available

        p.block = get_swap_block(); // get_swap_block() also reserves the block by setting it to True, so we can actually write back to it later
        p.file_backed = false;
        p.filename = nullptr;
    } else { // File-backed Page, BLOCK NUMBER matters
        p.block = block;
        p.file_backed = true;
        p.filename = filename;
    }

    unsigned int ppage = 0; // assign physical page 0 for now
    
    p.addr = (void*) ((uintptr_t)VM_ARENA_BASEADDR + (uintptr_t) VM_PAGESIZE * processes[current_pid].size);
    p.pid = current_pid;
    p.valid = true;
    p.resident = false;
    p.dirty = false;
    p.referenced = false;
    
    page_table_entry_t pte = {ppage, p.file_backed ? false : true, false};
    processes[current_pid].page_table[processes[current_pid].size++] = pte;                 // R/W Initialized to 00 or 10
    processes[current_pid].pages.push_back(std::move(p));                                   // Add p to the page meta data vector

    return (void*) processes[current_pid].pages.back().addr;
}


// Helper Functions

pid_t getpid() {
    return current_pid_num;
}

unsigned int addr_to_index(const void* addr) {
    return ((uintptr_t) addr - (uintptr_t) VM_ARENA_BASEADDR) >> 0x10;
}

void physical_memory_init() {
    assert(physical_pages[0] == nullptr);
    char* base_addr = (char*)vm_physmem;
    for (size_t i=0; i<VM_PAGESIZE; ++i) { // Setting the 0th Page to All Zeros
        *base_addr = 0;
        ++base_addr;
    }
}

void physical_pages_init() {
    for (size_t i=0; i<max_physical_pages; ++i) {
        physical_pages[i] = nullptr;
    }
}

bool physical_pages_full() { // This is slow (linear), can add a variable to keep track of it
    unsigned int sum = 1; // Page zero is pinned
    assert(physical_pages[0] == nullptr); // Page zero is pinned
    for (size_t i=1; i<max_physical_pages; ++i) {
        if (physical_pages[i] != nullptr) {
            ++sum;
        }
    }
    return sum >= max_physical_pages;
}

unsigned int get_physical_block() {
    if (!physical_pages_full()) { // If not full, get a block
        for (size_t i=1; i<max_physical_pages; ++i) { // Skip Page 0
            if (i != 0 && physical_pages[i] == nullptr) {
                assert(i != 0);
                return (unsigned int) i;
            }
        }
    } else {
        return evict_physical_page();
    }
    //throw std::runtime_error("No Physical Block is Available!");
    printf("No Physical Block is Available!");
    exit(-1);
    return 0;
}

bool swap_pages_full() {
    unsigned int sum = 0;
    for (size_t i=0; i<max_swap_pages; ++i) {
        if (swap_pages[i]) {
            ++sum;
        }
    }
    return sum >= max_swap_pages;
}

unsigned int get_swap_block() {
    for (size_t i=0; i<max_swap_pages; ++i) {
        if (!swap_pages[i]) {
            swap_pages[i] = true;
            return i;
        }
    }

    //throw std::runtime_error("No Swap Block is Available!");
    printf("No Swap Block is Available!");
    exit(-1);
    return 0;
}

unsigned int evict_physical_page() {
    printf("\n[N    O    W,    E    V    I    C    T    I    N    G]\n");

    assert(physical_pages.size() == max_physical_pages); // Sanity Check
    static size_t clock_idx = 1; // Used to track clock hand for the Clock Algorithm
    bool evicted = false; // IF true, then return
    
    // This will only run when the physical blocks are full
    // Worst case is that 100 pages are all referenced (1)
    // If uninterrupted, after 101 steps, there must be one that gets evicted
    // 100 Steps to set everyone to 0, 2 steps to skip page 0 2 times, and another step to evict the first one
    for (size_t i=0; i<max_physical_pages+3; ++i) { // FIFO with 2nd Chance
        unsigned int ppage = 1; // Page 0 is pinned
        if (clock_idx == 0) { // Page 0 is pinned and thus skipped
            clock_idx += 1;
            continue;
        }
        assert(clock_idx != 0); // Make sure Page 0 is not touched
        if (physical_pages[clock_idx] == nullptr && clock_idx != 0) { // Should not happen
            //throw std::runtime_error("All pages should have been valid!");
            printf("All pages should have been valid!");
            exit(-1);
        }  
        // Page is valid, check referenced bit
        if (physical_pages[clock_idx]->referenced) { // If referenced, set to not referenced (decrement)
            assert(clock_idx != 0);
            physical_pages[clock_idx]->referenced = false;
        } else { // Not referenced, evict this page
            assert(clock_idx != 0);
            printf("Will evict physical page #: %lu\n", clock_idx);
            //IARO_ADD i don't think this calculation is correct
            pid_t page_pid = physical_pages[clock_idx]->pid; // Get PID to locate the Page Table
            unsigned int idx = addr_to_index(physical_pages[clock_idx]->addr); // Calculate the PT Index from the virtual address
            unsigned int byte = processes[page_pid].page_table[idx].ppage * VM_PAGESIZE; // Calculate the corresponding byte

            physical_pages[clock_idx]->resident = false; // Mark the page non-resident
            processes[page_pid].page_table[idx].read_enable = false; // Mark the page non-resident
            processes[page_pid].page_table[idx].write_enable = false; // Mark the page non-resident

            if (physical_pages[clock_idx]->dirty) { // Only write back if dirty (Might be swap or file-backed page)
                    if(file_write(physical_pages[clock_idx]->filename, physical_pages[clock_idx]->block, (void*)(&((char*)vm_physmem)[byte])) == -1) {
                        printf("Writing to Regular File Failed with Return Value: -1");
                        exit(-1);
                        //throw std::runtime_error("Writing to Regular File Failed with Return Value: -1");
                    } // This works for both File-backed and Swap-backed Pages
            } // Now the page is written back or ignored
            physical_pages[clock_idx] = nullptr; // Set to nullptr
            ppage = clock_idx; // Set ppage to the current empty page
            evicted = true; // Page is evicted
        }// Eviction complete

        clock_idx = (clock_idx + 1) % max_physical_pages; // Advance Clock Hand
        if (evicted) {
            assert(ppage != 0);
            return ppage;
        }
    } // End of For Loop, should have returned

    //throw std::runtime_error
    printf("A Page should have already been evicted"); // Should return in the for loop
    exit(-1);
}

void create_empty_arena(pid_t current_pid) {
    std::vector<page_t> pages;
    pages.reserve(max_arena_pages);
    process_t current_process = {std::move(pages), {}, 0};
    // Process now contains an empty vector of page_t and an empty page table array
    processes[current_pid] = std::move(current_process);
}

void make_all_zero(unsigned int ppage) {
    assert(physical_pages[ppage] != nullptr); // Make sure the page is reserved
    char* base_addr = &((char*)vm_physmem)[ppage];
    for (size_t i=0; i<VM_PAGESIZE; ++i) { // Setting the 0th Page to All Zeros
        *base_addr = 0;
        ++base_addr;
    }
}

void print_virtual_page(page_t* page) {
    printf("#########[START VIRTUAL PAGE]#########\n");
    printf("Process ID:\t%d\n",page->pid);
    printf("Virtual Addr:\t%p\n",page->addr);
    printf("Block Number:\t%u\n",page->block);
    printf("Dirty Bit:\t%d\n",page->dirty);
    printf("File Backed?:\t%d\n",page->file_backed);
    printf("File Name:\t%p\n",page->filename);
    printf("Validity:\t%d\n",page->valid);
    printf("Residency:\t%d\n",page->resident);
    printf("Reference:\t%d\n",page->referenced);
    printf("---------[P       T       E]---------\n");
    printf("PPage:\t\t%u\n", processes[page->pid].page_table[addr_to_index(page->addr)].ppage);
    printf("Read Bit:\t%u\n", processes[page->pid].page_table[addr_to_index(page->addr)].read_enable);
    printf("Write Bit:\t%u\n", processes[page->pid].page_table[addr_to_index(page->addr)].write_enable);
    printf("#########[END OF VIRTUAL PAG]#########\n");
}

bool check_inside_arena(page_t* filename_page) {
    if ((uintptr_t) filename_page->addr < (uintptr_t) VM_ARENA_BASEADDR || (uintptr_t) filename_page->addr >= (uintptr_t) VM_ARENA_BASEADDR + (uintptr_t) VM_ARENA_SIZE) {
        return false;
    } else {
        assert(filename_page->resident == true); // Make sure the page is already resident
        unsigned int physical_block = processes[filename_page->pid].page_table[addr_to_index(filename_page->addr)].ppage;
        unsigned int byte = physical_block * VM_PAGESIZE;
        const char* filename = &((char*) vm_physmem)[byte];
        unsigned int length = strlen((const char*) filename);
        void* tmp = (void*) ((uintptr_t)filename_page->addr + length);
        if ((uintptr_t) tmp < (uintptr_t) VM_ARENA_BASEADDR || (uintptr_t) tmp >= (uintptr_t) VM_ARENA_BASEADDR + (uintptr_t) VM_ARENA_SIZE) {
            return false;
        }
    }
    return true;
}

const char* get_filename_physical(page_t* page) {
    pid_t pid = page->pid;
    assert(page->filename != nullptr);
    page_t* filename_page =&processes[pid].pages[addr_to_index(page->filename)]; // The page containing the filename
    assert(filename_page != nullptr);
    assert(filename_page->pid == pid);

    if (filename_page->valid == false) {
        //throw std::runtime_error
        printf("Trying to access invalid page!");
        exit(-1);
    }

    if (filename_page->resident == false) { // Page is non-resident
        if (make_page_resident(filename_page) == -1) {
            //throw std::runtime_error
            printf("Page cannot be accessed!");
            exit(-1);
        } // Now the Page should be Resident
    }

    
    unsigned int physical_block = processes[pid].page_table[addr_to_index(page->filename)].ppage;
    unsigned int byte = physical_block * VM_PAGESIZE;
    const char* filename = &((char*) vm_physmem)[byte];

    if (check_inside_arena(filename_page) == false) {
        //throw std::runtime_error
        printf("Filename outside the arena!");
        exit(-1);
    }
    return filename;
}

int make_page_resident(page_t* page) {
    assert(page->referenced == false);
    assert(page->resident == false);

    unsigned int ppage = get_physical_block();
    unsigned int idx = addr_to_index(page->addr);
    unsigned int byte = ppage * VM_PAGESIZE; // Calculate the corresponding byte
    
    processes[page->pid].page_table[idx].ppage = ppage; // Update PTE Physical Page
    assert(ppage != 0); // Make sure Page 0 is not touched
    physical_pages[ppage] = page; // Update Physical Pages Meta Data

    const char* physical_filename = (page->file_backed) ? get_filename_physical(page) : nullptr;
    if (file_read(physical_filename, page->block, (void*)(&((char*)vm_physmem)[byte])) == -1) { // Read Failed
        return -1;
    } else { // Read is successful, Mark resident, referenced, and change PTE Read Bit
        page->referenced = true;
        page->resident = true;
        assert(processes[page->pid].page_table[idx].read_enable == false);
        processes[page->pid].page_table[idx].read_enable = true; // Success, now page is read-only
        return 0;
    }
}

void print_physical_pages() {
    printf("$$$$$$$$$$$$$$$$$$$$[START PHYSICAL PAGES]$$$$$$$$$$$$$$$$$$$$\n");
    for (size_t i=0; i<max_physical_pages; ++i) {
        
        if (physical_pages[i] == nullptr) {
            if (i!=0) {
                printf("\n\n\n[E    M    P    T    Y]\n\n\n");
            } else {
                printf("\n\n\n[P   I   N   N   E   D]\n\n\n");
            }
        } else {
            print_virtual_page(physical_pages[i]);
        }

        if (i != max_physical_pages - 1) {
            printf("--------------------[ ↑↑ Page %lu ↓↓ Page %lu ]--------------------\n", i, i+1);
        }
        
    }
    printf("$$$$$$$$$$$$$$$$$$$$[END OF PHYSICAL PAGE]$$$$$$$$$$$$$$$$$$$$\n");
}