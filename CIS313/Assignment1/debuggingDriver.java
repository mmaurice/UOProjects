import java.util.Scanner;


public class debuggingDriver{

	public static void main(String[] args) {
    	System.out.print("Enter any string:");
        Scanner in=new Scanner(System.in);
        String inputString = in.nextLine();
        Queue queue = new Queue(inputString);
    
        String reverseString = "";

        while (!queue.isEmpty()) {
            reverseString = reverseString+queue.dequeue();
        }

        System.out.println("Reverse String " + reverseString);
        
        if (inputString.equals(reverseString))
            System.out.println("The input String is a palindrome.");
        else
            System.out.println("The input String is not a palindrome.");
            	}

}
