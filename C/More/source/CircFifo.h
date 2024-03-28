


class CircFifo {
public:
	CircFifo();
	uint16_t Push(uint8_t element);
	uint16_t Push(uint8_t *buf, uint16_t len);
	uint16_t Pop(uint8_t *ret);
	uint16_t Pop(uint8_t *buf, uint16_t len);
	uint8_t GetElement(uint16_t idx);
	uint16_t GetArray(uint16_t idx, uint8_t *buf, uint16_t len);
	bool Contains(uint8_t *str, uint8_t len);
	void Empty();

private:
	uint8_t buf[300]; //ptr to the start of the allocated fifo buffer
	uint8_t *head; //ptr to most recent element+1
	uint8_t *tail; //ptr to oldest element
	uint16_t count; //number of elements in fifo

	void fixTailHeadBounds();
	static bool strCmpLen(uint8_t *s1, uint8_t *s2, uint8_t len);

};


CircFifo::CircFifo() {
	head = buf;
	tail = buf;
	count = 0;
}

uint16_t CircFifo::Push(uint8_t element) {
	*(head) = element;

	head++;

	if(count==300)
		tail++;
	else
		count++;

	fixTailHeadBounds();

	return count;
}

uint16_t CircFifo::Push(uint8_t *buf, uint16_t len) {
	for(uint16_t i = 0; i < len; i++) {
		Push(buf[i]);
	}
	return count;
}

uint16_t CircFifo::Pop(uint8_t *ret) {
	if(count < 0)
		return count;

	head--;
	fixTailHeadBounds();
	*ret = *head;
	return count;
}

uint16_t CircFifo::Pop(uint8_t *buf, uint16_t len) {
	for(uint16_t i = 0; i < len; i++)
		Pop(buf+i);

	return count;
}

uint8_t CircFifo::GetElement(uint16_t idx) {
	if(idx > count)
		return 0xFF;

	if(tail + idx < buf + 299)
		return *(tail+idx);
	else
		return *(tail+idx-300);
}

uint16_t CircFifo::GetArray(uint16_t idx, uint8_t *buf, uint16_t len) {
	for(uint16_t i = 0; i < len; i++) {
		buf[i] = GetElement(idx+i);
	}
	return count;
}

bool CircFifo::Contains(uint8_t *str, uint8_t len) {
	uint8_t tmp[len];
	for(uint16_t i = 0; i < count-len+1; i++) {
		GetArray(i, tmp, len);
		if(strCmpLen(str, tmp, len))
			return true;
	}
	return false;
}

void CircFifo::fixTailHeadBounds() {
	if(tail>buf+299)
		tail = buf;
	else if (tail<buf)
		tail = buf+299;

	if(head>buf+299)
		head = buf;
	else if (head<buf)
		head = buf+299;
}

bool CircFifo::strCmpLen(uint8_t *s1, uint8_t *s2, uint8_t len) {
	for(uint8_t i = 0; i < len; i++) {
		if(s1[i] != s2[i])
			return false;
	}
	return true;
}

void CircFifo::Empty() {
	head = buf;
	tail = buf;
	count = 0;
}
