/*
 BST.java
 This is the Class containing the AVL Tree. It contains methods for 
 insert, find, remove, rotateRight, rotateLeft as well as 
 double rotations. 
 Muhammed Maurice
 24 November 2014
 */
public class BST {
	
	private BSTNode root;
//constructor
	public BST() {
		root = null;
	}
//check if tree is empty
	public boolean isEmpty(){
		return root == null;
	}
//insert a node into the catalog 
	public void insert(String data){
		root = insert(root, data);
	}
	private BSTNode insert(BSTNode node, String data){
		if (node == null)
			node = new BSTNode(data);
		else if (data.compareTo(node.getData()) <= 0){
			node.left = insert(node.left, data);
			if(height(node.left) - height(node.right) == 2)
				if(data.compareTo(node.left.getData()) <= 0)
					node = rotateLeft(node);
				else
					node = doublerotateLeft(node);
		}
		else if (data.compareTo(node.getData()) > 0){
			node.right = insert(node.right, data);
			if(height(node.right) - height(node.left) == 2)
				if(data.compareTo(node.right.getData()) > 0)
					node = rotateRight(node);
				else
					node = doublerotateRight(node);
		}
		else
			; //This means there is a duplicate, so do nothing
		node.height = max(height(node.left), height(node.right))+1;
		return node;
	}
//remove a book from the catalog
	public boolean remove(String x){
		boolean bool = false;
		if (isEmpty())
			System.out.println("Catalog is empty");
		else if (find(x) == false)
			bool = false;
			//System.out.println(x + " is not in the catalog");
		else{
			root = remove(root, x);
			bool = true;
			//System.out.println(x + " has been removed from the catalog");
		}
		return bool;
	}
	public BSTNode remove(BSTNode root, String k){
		BSTNode p, p2, n;
		if ((root.getData()).compareTo(k) == 0 ){
			BSTNode lt, rt;
			lt = root.getLeft();
			rt = root.getRight();
			if(lt == null && rt == null)
				return null;
			else if (lt == null){
				p = rt;
				return p;
			}
			else if (rt == null){
				p = lt;
				return p;
			}
			else{
				p2 = rt;
				p = rt;
				while (p.getLeft() != null)
					p = p.getLeft();
				p.setLeft(lt);
				return p2;
			}
		}
		if (k.compareTo(root.getData()) < 0){
			n = remove(root.getLeft(), k);
			root.setLeft(n);
		}
		else{
			n = remove(root.getRight(), k);
			root.setRight(n);
		}
		return root;
	}
	
//finds a book in the catalog
	public boolean find(String title){
		return search(root, title);
	}
	public boolean search(BSTNode t, String title){
		boolean found  = false;
		
		while ((t != null) && !found){
			String rtitle = t.getData();
			if (title.compareTo(rtitle) < 0)
				t = t.getLeft();
			else if (title.compareTo(rtitle) > 0)
				t = t.getRight();
			else{
				found = true;
				break;
			}
		}
		return found;
	}
	

	//additions for lab 3
	//function to get height of node
	private int height(BSTNode t)
	{
		return t==null ? -1:t.height;
	}
	//function to get max of left/right node
	private int max(int left, int right)
	{
		return left > right ? left:right;
	}
	
	//perform a left rotation around node
	private BSTNode rotateLeft(BSTNode b)
	{
		BSTNode a;
		a = b.left;
		b.left = a.right;
		a.right = b;
		b.height = max(height(b.left), height(b.right))+1;
		a.height = max(height(a.left), b.height)+1;
		return a;
	}
	
	private BSTNode rotateRight(BSTNode a)
	{
		BSTNode b;
		b = a.right;
		a.right = b.left;
		b.left = a;
		a.height = max(height(a.left), height(a.right))+1;
		b.height = max(height(b.right), a.height)+1;
		return b;
	}
	
	//Double rotation methods for both left and right nodes
	
	private BSTNode doublerotateLeft(BSTNode c)
	{
		c.left = rotateRight(c.left);
		return rotateLeft(c);
	}
	private BSTNode doublerotateRight(BSTNode a)
	{
		a.right = rotateLeft(a.right);
		return rotateRight(a);
	}

}







