#ifndef STACK_H
#define STACK_H
template<class T, class TContainer>
class Stack 
{
	TContainer data;
public:
    Stack(){}
    Stack(int n) {data.reserve(n);}
	void push(const T& el) {data.push_back(el);}; // O(1)
	void pop() {data.pop_back();}; // O(1)
	T top() {return data[data.size() - 1];} // O(1)
	size_t size() const {return data.size();};
	void clear() {data.clear();};
	bool empty() const {return data.empty();};
};
#endif