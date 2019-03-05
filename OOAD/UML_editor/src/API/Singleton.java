package API;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;

import Graph.Shape;

public class Singleton{
	private volatile static Singleton uniqueInstance = new Singleton(); 
	
	private Singleton(){}
	
	public static Singleton getInstance(){
		return uniqueInstance;
	}
	
    //sort depth using quick sort
    public void sort(ArrayList<Shape> list){
    	Collections.sort(list, getDepthByGraph());
    }
    
    public Comparator<Shape> getDepthByGraph()
    {
	     Comparator<Shape> comp = new Comparator<Shape>(){
	         public int compare(Shape s1, Shape s2){
	             return s2.getDepth() - s1.getDepth();
	         }
	     };
	     return comp;
    }
    
}
