/*Muhammed Maurice
 * CIS 315
 * Chris Wilson
 * Homework 2
 * 
 * This program can be compiled with
 * javac Driver.java
 * And run with
 * java Driver file.txt
 */
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.InputMismatchException;
import java.util.LinkedList;
import java.util.Queue;
import java.util.Scanner;

public class Driver {

	public static void main(String[] args) throws IOException {
		File file =  new File(args[0]);

		 int number_of_vertices, x = 1;
		 BufferedReader reader = new BufferedReader( new FileReader(file));

	        GraphAdjacencyMatrix adjacencyMatrix;
	 
	        try 
	        {  
	            String line;
	            line  = reader.readLine();
	        	int number_of_graphs = Integer.parseInt(line);
	            while(x <= number_of_graphs){
	            	int count = 1;
		        	line  = reader.readLine();
		            number_of_vertices =  Integer.parseInt(line);
		            //System.out.println(number_of_vertices);
		            line  = reader.readLine();
		            int number_of_edges =  Integer.parseInt(line);
		            //System.out.println(number_of_edges);
		            adjacencyMatrix = new GraphAdjacencyMatrix(number_of_vertices);
		   		 	int[][] adjmatrix = new int[number_of_vertices+1][number_of_vertices+1];

			           // System.out.println("Enter The Graph Egdes :<source> <destination>");
		            while (count <= number_of_edges)
		            {
		            	line  = reader.readLine();
		            	String[] lines = line.split(" ");
		                int source = Integer.parseInt(lines[0]);
		                int destination = Integer.parseInt(lines[1]);
		                adjacencyMatrix.setEdge(source, destination, 1);
		                count++;
		            }
		            System.out.println("graph number: " + x);
		           // for (int i = 1; i <= number_of_vertices; i++)
		             //   System.out.print(i);
		 
		            for (int i = 1; i <= number_of_vertices; i++) 
		            {
		                //System.out.print(i);
		                for (int j = 1; j <= number_of_vertices; j++)
		                { 
		                    //System.out.print(adjacencyMatrix.getEdge(i, j));
		                	
		                	//populate adjacency matrix with 0s and 1s
		                    adjmatrix[i][j] =adjacencyMatrix.getEdge(i, j);
		                } 
		                //System.out.println();
		            }
		            Graph G = new Graph(adjmatrix);
		            G.bfs();
		            x++;
	            }
	        } catch (InputMismatchException inputMisMatch) 
	        {
	            System.out.println("Error in Input Format.<source index> <destination index>");
	        }
	        reader.close();
	    }

}

//---------------------------------------------------------------------


class GraphAdjacencyMatrix
{
    private final int MAX_NO_OF_VERTICES;
    private int adjacency_matrix[][];
 
    public GraphAdjacencyMatrix(int number_of_vertices)
    {
        MAX_NO_OF_VERTICES = number_of_vertices;
        adjacency_matrix = new int[MAX_NO_OF_VERTICES + 1][MAX_NO_OF_VERTICES + 1];
    }
 
    public void setEdge(int from_vertex, int to_vertex, int edge) 
    {
        try 
        {
            adjacency_matrix[from_vertex][to_vertex] = edge;
        } catch (ArrayIndexOutOfBoundsException indexBounce)
        {
            System.out.println("the vertex entered is not present");
        }
    }
 
    public int getEdge(int from_vertex, int to_vertex)
    {
        try 
        {
            return adjacency_matrix[from_vertex][to_vertex];
        } catch (ArrayIndexOutOfBoundsException indexBounce)
        {
            System.out.println("the vertex entered is not present");
        }
        return -1;
    }
}



//------------------------------------------------------------------

class Graph
{

   int[][]  adjMatrix;
   int      rootNode = 1;
   int		shortest = 0;
   int      longest = 0;
   int		total = 0;
   int      head = 0;
   int      NNodes;

   boolean[] visited; 

   Graph(int N)
   {
      NNodes = N;
      adjMatrix = new int[N][N];
      visited = new boolean[N];
   }

   Graph(int[][] mat)
   {
      int i, j;

      NNodes = mat.length;

      adjMatrix = new int[NNodes][NNodes];
      visited = new boolean[NNodes];


      for ( i=1; i < NNodes; i++){
         for ( j=1; j < NNodes; j++){
             adjMatrix[i][j] = mat[i][j];
        	 //System.out.print(adjMatrix[i][j]);
         }
      }
   }


   public void bfs()
   {
      // BFS uses Queue data structure
      Queue<Integer> q = new LinkedList<Integer>();
      int[] totalPaths = new int[NNodes-2];
      int i = 0;
      q.add(rootNode);
      visited[rootNode] = true;
      while( !q.isEmpty() )
      {
    	 //System.out.println("here");
         int n, child;
         n = (q.peek()).intValue();
         child = getUnvisitedChildNode(n);
         //printNode(child);

         if ( child != -1 )
         {
            visited[child] = true;
            //printNode(child);
            q.add(child);
            head = q.peek();
           /* for(int s : q) { 
            	  System.out.print(s); 
            	}*/
            if(q.size() - 1 == 1){
            	shortest++;
            }
            totalPaths[i] = q.size()-1;
            i++;
            //System.out.println("\n"+ q.size());
            //System.out.println("the queue is here");
         }
         else
         {
            q.remove();
            longest++;
        	//System.out.println("here");
         }
      }

      for(int x = 0; x<totalPaths.length-1; x++){
    	  if(totalPaths[x+1] == 1)
    		  totalPaths[x+1] = totalPaths[x];
    	  else
    		  totalPaths[x+1] = totalPaths[x]*2;
      }
     /* for(int x = 0; x<totalPaths.length-1; x++){
    	  System.out.print(totalPaths[x]); 
      }*/
      if(totalPaths.length-1 <= 0)
    	  total = 1;
      else
    	  total = totalPaths[totalPaths.length-1];
      
      System.out.println("Shortest path: "+ shortest);
      System.out.println("Longest path: "+ (longest-1));
      System.out.println("Number of paths: " + total);
      System.out.println();
      clearVisited();      //Clear visited property of nodes
   }
   
   int getUnvisitedChildNode(int n)
   {
      int j;
      for ( j = 1; j < NNodes; j++ )
      {
    	  if ( adjMatrix[n][j] > 0 )
          {
    		  if ( ! visited[j] )
               return(j);
          }
      }

      return(-1);
   }

   void clearVisited()
   {
      int i;

      for (i = 1; i < visited.length; i++)
         visited[i] = false;
   }

   void printNode(int n)
   {
      System.out.println(n);
   }

}

