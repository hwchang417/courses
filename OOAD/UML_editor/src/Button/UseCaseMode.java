package Button;

import java.awt.Graphics;
import java.awt.event.MouseEvent;
import java.util.ArrayList;

import GUI.UML_Canvas;
import Graph.Shape;
import Graph.UseCase;

public class UseCaseMode extends Mode{

	public UseCaseMode(UML_Canvas canvas){
		super(canvas);
	}
	
	@Override
	public void mouseDragged(MouseEvent e) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void mouseMoved(MouseEvent e) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void mouseClicked(MouseEvent e) {
		// TODO Auto-generated method stub
	}

	@Override
	public void mouseEntered(MouseEvent e) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void mouseExited(MouseEvent e) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void mousePressed(MouseEvent e) {
		// TODO Auto-generated method stub
    	ArrayList<Shape> list = _canvas.getList();
    	int x, y, d;
    	x = e.getX();
    	y = e.getY();
    	
    	//check if there is any class in area, and get depth of this area 
    	d = checkDepth(x, y);

    	list.add(new UseCase(d, x, y));
    	_canvas.setList(list);
    	_canvas.repaint();
	}

	@Override
	public void mouseReleased(MouseEvent e) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void drawGraph(Graphics g) {
		// TODO Auto-generated method stub
		
	}

}
