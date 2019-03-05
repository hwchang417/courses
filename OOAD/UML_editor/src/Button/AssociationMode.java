package Button;

import java.awt.Graphics;
import java.awt.event.MouseEvent;

import GUI.UML_Canvas;

public class AssociationMode extends LineMode{

	public AssociationMode(UML_Canvas canvas){
		super(canvas);
	}
	
	@Override
	public void mouseDragged(MouseEvent e) {
		// TODO Auto-generated method stub
		mouseDragged(e.getX(), e.getY());
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
		int x = e.getX();
		int y = e.getY();
		int d = checkDepth(x, y);
		mousePressed(d, x, y);
	}

	@Override
	public void mouseReleased(MouseEvent e) {
		// TODO Auto-generated method stub

		int x = e.getX();
		int y = e.getY();
		int d = checkDepth(x, y);
		mouseReleased(d, x, y, LineMode.TYPE_ASSOCIATION);
	}

	@Override
	public void drawGraph(Graphics g) {
		// TODO Auto-generated method stub
    	drawGraph(g, LineMode.TYPE_ASSOCIATION);
	}
}
