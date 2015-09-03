/*
BSTNode.java
This is the node class for BST and it contains basic getters and setters
the nodes, data, and height. 
Muhammed Maurice
November 2014
 */
public class BSTNode {
	BSTNode left, right, parent;
	String data;
	int height;
	
	public BSTNode(){
		left = null;
		right = null;
		parent = null;
		data = "";
		height = 0;
	}
	
	public BSTNode(String data){
		left = null;
		right = null;
		parent = null;
		this.data = data;
	}
	
	public void setHeight(int n)
	{
        height = n;
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
    
    public int getHeight()
    {
	    return height;
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
