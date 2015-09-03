/*
 BST.java
 This is the Class containing the Binary Search Tree. It contains methods for 
 insert, find, and remove. 
 Muhammed Maurice
 7 November 2014
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
		else{
			if(data.compareTo(node.getData()) <= 0)
				node.left = insert(node.left, data);
			else
				node.right = insert(node.right, data);
		}
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
	
	
}
