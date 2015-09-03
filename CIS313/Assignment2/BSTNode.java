/*
BSTNode.java
This is the node class for BST and it contatins basic getters and setters
the nodes and data. 
 Muhammed Maurice
 7 November 2014
 */
public class BSTNode {
	BSTNode left, right, parent;
	String data;
	
	public BSTNode(){
		left = null;
		right = null;
		parent = null;
		data = "";
	}
	
	public BSTNode(String data){
		left = null;
		right = null;
		parent = null;
		this.data = data;
	}
	
	public void setLeft(BSTNode n)
	{
        left = n;
    }

    public void setRight(BSTNode n)
    {
        right = n;
    }
    public void setParent(BSTNode n)
    {
        parent = n;
    }
    
    public BSTNode getLeft()
    {
	    return left;
    }
    public BSTNode getRight()
    {
 	    return right;
    }
    public BSTNode getParent()
    {
 	    return parent;
    }
    public void setData(String d)
    {
	    data = d;
    }
    public String getData()
    {
	    return data;
    } 
    
}
