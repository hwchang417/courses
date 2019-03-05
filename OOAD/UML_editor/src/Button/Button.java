package Button;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.Icon;
import javax.swing.JButton;

import GUI.UML_Canvas;

public abstract class Button extends JButton implements ActionListener{

	private static final long serialVersionUID = 1L;
	public static final Button NOSELECTED = null;
	private Color _pressedBackgroundColor;
	protected boolean _state;
    protected UML_Canvas _canvas;
    
    public Button(String text, Icon icon, Dimension dimension, Color color, UML_Canvas canvas) {
        super(text, icon);
        super.setContentAreaFilled(false);
        this.setPreferredSize(dimension);
        this.setPressedBackgroundColor(color);
        this.addActionListener(this);
        this._state = false;
        this._canvas = canvas;
    }
    
    public abstract void actionPerformed(ActionEvent e);
    
    protected void changeMode(Mode new_mode){
		// change the mode when the button is pressed
		if(this.getState()){
			this._canvas.setMode(Mode.NOSELECTED);
			this.setState(false);
		}else{
			Mode mode = this._canvas.getMode();
			if(mode != Mode.NOSELECTED && mode.getButton() != Button.NOSELECTED){
				mode.getButton().setState(false);
				mode.getButton().repaint();
			}
			mode = new_mode;
			mode.setButton(this);
			this.setState(true);
			this._canvas.setMode(mode);
		}
    }
    
    @Override
    protected void paintComponent(Graphics g) {
        if (_state) {
            g.setColor(_pressedBackgroundColor);
        } else {
            g.setColor(getBackground());
        }
        g.fillRect(0, 0, getWidth(), getHeight());
        super.paintComponent(g);
    }

    @Override
    public void setContentAreaFilled(boolean b) {}

    public Color getPressedBackgroundColor() {
        return _pressedBackgroundColor;
    }

    public void setPressedBackgroundColor(Color pressedBackgroundColor) {
        this._pressedBackgroundColor = pressedBackgroundColor;
    }
    
    public void setState(boolean state){
    	this._state = state;
    }

    public boolean getState(){
    	return _state;
    }
}
