package Graph;

import java.awt.FontMetrics;
import java.awt.Graphics;

public class UseCase extends BasicObject{
	public static final int GRAPH_WIDTH = 100;
	public static final int GRAPH_HEIGHT = 50;
	
	private Point _drag_diff;
	
	public UseCase(int d, int x, int y){
		super(d, x, y, GRAPH_WIDTH, GRAPH_HEIGHT);
		_drag_diff = new Point(0, 0);
	}
	
	public void draw(Graphics g){
		int center_x = _position.x + (_width >> 1);
		int center_y = _position.y + (_height >> 1);
		
        // Draw oval
        g.setColor(_background);
        g.fillOval(_position.x, _position.y, _width, _height);

        g.setColor(_foreground);
        g.drawOval(_position.x, _position.y, _width, _height);
        
        // Draw centered text
        FontMetrics fm = g.getFontMetrics();
        double textWidth = fm.getStringBounds(_title, g).getWidth();
        
        g.drawString(_title, (int) (center_x - textWidth/2),
                           (int) (center_y  + fm.getMaxAscent() / 2));
        
        //check selected
        showConnectedPort(g);
	}

	public boolean isInside(int x, int y){
		int width_2 = _width >> 1;
		int height_2 = _height >> 1;
		int center_x = _position.x + width_2;
		int center_y = _position.y + height_2;
		int tmpx = x - center_x;
		int tmpy = y - center_y;
		
		return ((float)(tmpx * tmpx)/(width_2 * width_2) + (float)(tmpy * tmpy)/(height_2 * height_2)) < 1.0f;
	}
	
	@Override
	public boolean isOutside(int fx, int fy, int tx, int ty) {
		// TODO Auto-generated method stub
		int width_2 = _width >> 1;
		int height_2 = _height >> 1;
		int center_x = _position.x + width_2;
		int center_y = _position.y + height_2;
		int leftx = center_x - width_2;
		int topy = center_y - height_2;
		int rightx = center_x + width_2;
		int bottomy = center_y + height_2;
		
		return fx <= leftx && fy <= topy && tx >= rightx && ty >= bottomy;
	}

	@Override
	public void startDragging(int x, int y) {
		// TODO Auto-generated method stub
		_drag_diff.x = _position.x - x;
		_drag_diff.y = _position.y - y;
	}

	@Override
	public void doDragging(int x, int y) {
		// TODO Auto-generated method stub
		_position.x = _drag_diff.x + x;
		_position.y = _drag_diff.y + y;
	}

	@Override
	public int getX() {
		// TODO Auto-generated method stub
		return _position.x;
	}

	@Override
	public int getY() {
		// TODO Auto-generated method stub
		return _position.y;
	}

	@Override
	public int getWidth() {
		// TODO Auto-generated method stub
		return GRAPH_WIDTH;
	}

	@Override
	public int getHeight() {
		// TODO Auto-generated method stub
		return GRAPH_HEIGHT;
	}

}
