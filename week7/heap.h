#ifndef HEAP_H_INCLUDED
#define HEAP_H_INCLUDED
const int MAXN = 100010;

class array {
	private:
	int elem[MAXN];

	public:
	int &operator[](int i) { return elem[i]; }
};

class heap {
	private:
	int n;
	array h;

	public:
	void clear() { n = 0; }
	int top() { return h[1]; }
	int size() { return n; }
	void push(int);
	void pop();
};
#endif // HEAP_H_INCLUDED