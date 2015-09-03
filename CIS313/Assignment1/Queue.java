
public class Queue {	
	public CharNode frontNode = null;
	public CharNode rearNode = null;
	
	public Queue(String pstring){
		for(char i : pstring.toCharArray()){
			CharNode c = new CharNode(i,null);
			enqueue(i);
		}
	}
	//The enqueue method adds a node to the rear of the queue
	public void enqueue(char input) {
		CharNode temp = new CharNode(input, null);
		if (isEmpty())
			frontNode = temp;
		else
			rearNode.next = temp;
		
		rearNode = temp;
	}
	//The dequeue method takes a node from the front of the queue
	public char dequeue(){
		if (isEmpty()){
			new Exception("Queue is empty");
		}
		char result = frontNode.data;
		if (rearNode == frontNode)
			rearNode = null;
		frontNode = frontNode.next;
		return result;
	}
	
	public boolean isEmpty(){
		return (frontNode == null);
	}
}
