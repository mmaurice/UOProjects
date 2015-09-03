import java.util.EmptyStackException;


public class Stack{
	public CharNode topNode;
	public Stack(String pstring){
		for(char i : pstring.toCharArray()){
			CharNode c = new CharNode(i,null);
			push(i);
		}
	}
//the pop method removes a element from the top of the stack
	public char pop() {
		if (isEmpty()){
			new Exception("Stack is empty");
		}
		char result = topNode.data;
		topNode = topNode.next;
		return result;
	}
//the push method adds an element to the top of the stack and redefines what the top elm is.
	public void push(char z) {
		CharNode temp = new CharNode();
		temp.data = z;
		temp.next = topNode;
		topNode = temp;
	}
	public boolean isEmpty() {
		return (topNode == null);
	}
}
