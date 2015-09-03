/*
 LibraryDriver.java
 This is the driver file for the libraries book catalog. 
 BST functions are implemented here.
 Muhammed Maurice
 7 November 2014
 */
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Scanner;


public class LibraryDriver {

	public static void main(String[] args) throws IOException {
        String[] results;
        String[] catalog;
        int m;
        BST bst = new BST();
        
        File file = new File(args[0]);
        BufferedWriter out = new BufferedWriter(new FileWriter(args[1]));
        
        Scanner stdin = new Scanner(file);
        
        m = stdin.nextInt()*2;
       
        //System.out.println(m);
        results = new String[m+1];
        catalog = new String[(m)+1];

        int key = 0;
        while (key <= m){
        	String str = stdin.nextLine();
        	results[key] = str;
        	//System.out.println(results[key]);
        	key++;
        }
        int i = 1;
        while (i <= m){
        	if (results[i].compareToIgnoreCase("insert") == 0){
        		bst.insert(results[i+1]);
        		catalog[i] = "Inserted " + results[i+1] + " into the catalog.";
        	}
        	else if (results[i].compareToIgnoreCase("remove") == 0){
        		boolean val1 = bst.remove(results[i+1]);
        		if (val1 == true)
        			catalog[i] =  results[i+1] + " was removed from the catalog.";
        		else
        			catalog[i] =  results[i+1] + " is not in the catalog.";
        	}
        	else if (results[i].compareToIgnoreCase("find") == 0){
        		boolean val2 = bst.find(results[i+1]);
        		if (val2 == true)
        			catalog[i] =  results[i+1] + " is in the catalog.";
        		else
        			catalog[i] =  results[i+1] + " is not in the catalog.";
        	}

        	i+=2;
        }
        
        for (int x = 1; x < m; x+=2){
        	//System.out.println(catalog[x]);
        	out.write(catalog[x]);
        	out.newLine();
        }
        out.close();
	}

}
