package Button;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.event.ActionEvent;

import javax.swing.ImageIcon;

import GUI.UML_Canvas;

public class CompositionBtn extends Button{

	private static final long serialVersionUID = 1L;

    private static final String _img_name = "image/composition line.png";
	private static final Color BUTTON_PRESSED_COLOR = Color.GRAY;
    private static final int BUTTON_WIDTH = 80, BUTTON_HEIGHT = 80;
	
	public CompositionBtn(UML_Canvas canvas){
		super("", 
				new ImageIcon(_img_name), 
				new Dimension(BUTTON_WIDTH, BUTTON_HEIGHT), 
				BUTTON_PRESSED_COLOR,
				canvas);
	}

	@Override
	public void actionPerformed(ActionEvent e) {
		// TODO Auto-generated method stub
		_canvas.unSelectAll();
		changeMode(new CompositionMode(_canvas));
	}
}
