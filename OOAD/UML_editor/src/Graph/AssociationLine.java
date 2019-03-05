package Graph;

import java.awt.Graphics;

public class AssociationLine extends Line{

	public AssociationLine(int d, Port start, Port end) {
		super(d, start, end);
		// TODO Auto-generated constructor stub
	}

	@Override
	public void draw(Graphics g) {
		// TODO Auto-generated method stub
		Point start_offset = getOffset(_start);
		Point end_offset = getOffset(_end);
		int sx, sy, ex, ey;
		
		sx = _start._obj.getX() + start_offset.x;
		sy = _start._obj.getY() + start_offset.y;
		ex = _end._obj.getX() + end_offset.x;
		ey = _end._obj.getY() + end_offset.y;
		
		drawLineFromPoint(g, sx, sy, ex, ey);
	}
	
	
	public static void drawLineFromPoint(Graphics g, int sx, int sy, int ex, int ey){
		//draw end symbol by its type
		g.setColor(LINK_FOREGROUND);
		drawPort(g, sx, sy);
		//draw Link from start to end
		g.drawLine(sx, sy, ex, ey);
	}
}
