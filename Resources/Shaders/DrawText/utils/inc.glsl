
int getByte(in uint value, int offset) {
	return int(value >> (offset * 8) & 0x000000ff);
}

