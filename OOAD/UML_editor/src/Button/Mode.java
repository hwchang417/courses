package Button;

import java.awt.Graphics;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.util.ArrayList;

import API.Singleton;
import GUI.UML_Canvas;
import Graph.Shape;

public abstract class Mode implements MouseListener, MouseMotionListener{

	public static final Mode NOSELECTED = null;
	public static final int MAX_DEPTH = 99;
	protected Button _btn;
	protected UML_Canvas _canvas;
	
	public Mode(UML_Canvas canvas){
		this._btn = Button.NOSELECTED;
		_canvas = canvas;
	}
	
	public Button getButton(){
		return _btn;
	}

	public void setButton(Button btn){
		this._btn = btn;
	}
	
	public abstract void mouseDragged(MouseEvent arg0);
	public abstract void mouseMoved(MouseEvent arg0);
	public abstract void mouseClicked(MouseEvent arg0);
	public abstract void mouseEntered(MouseEvent arg0);
	public abstract void mouseExited(MouseEvent arg0);
	public abstract void mousePressed(MouseEvent arg0);
	public abstract void mouseReleased(MouseEvent arg0);
	public abstract void drawGraph(Graphics g);
	
    protected int checkDepth(int x, int y){
    	int min_depth = MAX_DEPTH + 1;
    	ArrayList<Shape> list = _canvas.getList();
    	for (Shape g: list) {
    		if(min_depth > g.getDepth()){
    			min_depth = g.getDepth();
    		}
		}
    	
    	return min_depth - 1;
    }
    
    protected void shrinkDepth(){
		//5, 7, 30, 99 => 96, 97, 98, 99
		int depth = MAX_DEPTH;
    	ArrayList<Shape> list = _canvas.getList();
    	
		Singleton.getInstance().sort(list);
		for (Shape g: list) {
			g.setDepth(depth--);
		}
	}
	
    protected void unSelectAll(){
    	ArrayList<Shape> list = _canvas.getList();
		for (Shape g: list) {
			g.unSelected();
		}
	}
	
    protected Shape getTopObject(int x, int y){
		int min_depth = MAX_DEPTH + 1;
    	ArrayList<Shape> list = _canvas.getList();
    	Shape top = null;
		
		for (Shape g: list) {
			if(g.isInside(x, y)){
	    		if(min_depth > g.getDepth()){
	    			min_depth = g.getDepth();
	    			top = g;
	    		}
			}
		}
    	
		return top;
	}

}
