/**
 *
 * PalindromeDriver.java
 *
 * The main driver code for lab 1 (asst2).
 * Uses user-defined stack and queue class to determine if
 * input words are palindromes.
 *
 */ 

import java.io.IOException;
import java.util.Scanner;

public class PalindromeDriver {

    /* Main method: reads input and calls palindrome test, prints results */
    public static void main(String[] args) throws IOException
    {
        String[] results;
        int num_phrases;

        Scanner stdin = new Scanner(System.in);
        String line;
        num_phrases = Integer.parseInt(stdin.nextLine());
	//System.out.println("im here");
        results = new String[num_phrases];
        String alphaOnly;
        
        int i=0;
        while (stdin.hasNextLine()) {
        	line = stdin.nextLine();
        	alphaOnly = line.replaceAll("[^a-zA-Z]+","");
        	if (is_palindrome(alphaOnly) == true)
        		results[i] = "Palindrome";
        	else
        		results[i] = "Not a palindrome";
        i++;
        }
        
        //TODO Write a loop that reads num_phrases input lines from stdin,
        //     and checks whether each line is a palindrome or not.
        //     Use the is_palindrome subroutine to check each line....
        //     you will have to do some preprocessing to the lines first.
        //     Store the results of each line in results array.
        for (int j = 0; j < num_phrases; j++){
        	System.out.println(results[j]);
        }
        //TODO Write a loop that iterates through the results array and
        //     prints all the results to stdout.
    }

    /* is_palindrome: checks whether input is palindrome, returns true if so,
     *                false otherwise.
     */
    public static boolean is_palindrome(String input) {
    	Stack s = new Stack(input);
    	Queue q = new Queue(input);
    	
    	while (!s.isEmpty() | !q.isEmpty())
    		if (q.dequeue() != s.pop())
    			return false;
    	return true;
        //TODO Write the body of is_palindrome,
        //     which uses a stack and a queue (data structure classes defined
        //     by you) to check whether the input is a palindrome.
        //     Return true if so, false otherwise.
    }
}