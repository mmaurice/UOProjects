/*This is a class that defines the nodes for the stack and queue*/
public class CharNode {
	char data;
	CharNode next;
	
	public CharNode(){
		data = ' ';
		next = null;
	}
	public CharNode(char data, CharNode next){
		this.data = data;
		this.next = next;
	}
}
