import java.awt.BorderLayout;
import java.awt.FlowLayout;

import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JPanel;
import javax.swing.border.EmptyBorder;
import javax.swing.JLabel;
import javax.swing.JTextField;
import javax.swing.JRadioButton;
import javax.swing.JComboBox;
import javax.swing.ButtonGroup;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import javax.swing.JTextArea;
import javax.swing.JTextPane;
import java.awt.Font;
import java.awt.Color;

public class MatchesGui extends JDialog {

	private final JPanel contentPanel = new JPanel();
	private MatchesImpl matchesImpl = null;

	/**
	 * Launch the application.
	 */
	public static void main(String[] args) {
		try {
			MatchesGui dialog = new MatchesGui();
			dialog.setDefaultCloseOperation(JDialog.DISPOSE_ON_CLOSE);
			dialog.setVisible(true);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	/**
	 * Create the dialog.
	 */
	public MatchesGui() {
		getContentPane().setEnabled(false);
		setBounds(100, 100, 486, 300);
		getContentPane().setLayout(null);
		contentPanel.setBounds(0, 0, 450, 1);
		contentPanel.setBorder(new EmptyBorder(5, 5, 5, 5));
		getContentPane().add(contentPanel);
		contentPanel.setLayout(null);
		{
			JPanel buttonPane = new JPanel();
			buttonPane.setBounds(0, 228, 450, 35);
			buttonPane.setLayout(new FlowLayout(FlowLayout.RIGHT));
			getContentPane().add(buttonPane);
			{
				JButton okButton = new JButton("OK");
				okButton.setActionCommand("OK");
				buttonPane.add(okButton);
				getRootPane().setDefaultButton(okButton);
			}
			{
				JButton cancelButton = new JButton("Cancel");
				cancelButton.setActionCommand("Cancel");
				buttonPane.add(cancelButton);
			}
		}
		
		JLabel lblMatches = new JLabel("Matches");
		lblMatches.setBounds(99, 29, 70, 15);
		getContentPane().add(lblMatches);
		
		JTextField matches = new JTextField();
		matches.setBounds(187, 22, 114, 30);
		getContentPane().add(matches);
		matches.setColumns(10);
		
		JRadioButton rdbtnUserChoice1 = new JRadioButton("User takes 1 match");
		rdbtnUserChoice1.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				matchesImpl.setUserChoice(1);
			}
		});
		rdbtnUserChoice1.setBounds(55, 60, 189, 23);
		getContentPane().add(rdbtnUserChoice1);
		
		JRadioButton rdbtnUserChoice2 = new JRadioButton("User takes 2 matches");
		rdbtnUserChoice2.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				matchesImpl.setUserChoice(2);
			}
		});
		rdbtnUserChoice2.setBounds(55, 90, 189, 23);
		getContentPane().add(rdbtnUserChoice2);
		
		JRadioButton rdbtnUserChoice3 = new JRadioButton("User takes 3 matches");
		rdbtnUserChoice3.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				matchesImpl.setUserChoice(3);
			}
		});
		rdbtnUserChoice3.setBounds(55, 117, 189, 23);
		getContentPane().add(rdbtnUserChoice3);
		
		JRadioButton rdbtnUserChoice4 = new JRadioButton("User takes 4 matches");
		rdbtnUserChoice4.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				matchesImpl.setUserChoice(4);
			}
		});
		rdbtnUserChoice4.setBounds(55, 142, 189, 23);
		getContentPane().add(rdbtnUserChoice4);
		
		ButtonGroup userChoices = new ButtonGroup();
		userChoices.add(rdbtnUserChoice1);
		userChoices.add(rdbtnUserChoice2);
		userChoices.add(rdbtnUserChoice3);
		userChoices.add(rdbtnUserChoice4);
		
		JTextArea txtrMessage = new JTextArea();
		txtrMessage.setEditable(false);
		txtrMessage.setBackground(new Color(238, 238, 238));
		txtrMessage.setFont(new Font("Dialog", Font.BOLD, 12));
		txtrMessage.setText("1. Enter initial Matches\n2. Press User Play Done");
		txtrMessage.setBounds(252, 94, 186, 66);
		getContentPane().add(txtrMessage);

		JButton btnPlayCompleted = new JButton("User Play Done");
		btnPlayCompleted.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				if (matches.isEditable()) {
					// Initial setup
					matches.setEditable(false);
					matchesImpl = new MatchesImpl();
					matchesImpl.setNumMatches(Integer.parseInt(matches.getText()));
				}
				
				int matchesLeftAfterUserPlay = matchesImpl.getNumMatches();
				if (matchesLeftAfterUserPlay == 0) {
					txtrMessage.setText("User wins!");
					return;
				}
				
				int robotPlay = matchesImpl.robotPlay();
				int matchesLeftAfterRobotPlay = matchesImpl.getNumMatches();
				matches.setText(Integer.toString(matchesLeftAfterRobotPlay));
				
				if (matchesLeftAfterRobotPlay > 0) {
					txtrMessage.setText(Integer.toString(matchesLeftAfterUserPlay) + 
							            " left after user turn\n" +
				                        "Robot takes " + Integer.toString(robotPlay) +  "\n" +
				                        Integer.toString(matchesLeftAfterRobotPlay) + 
				                        " left\nUser turn");
				} else {
					txtrMessage.setText(Integer.toString(matchesLeftAfterUserPlay) + 
				            " left after user turn\n" +
	                        "Robot takes " + Integer.toString(robotPlay) +  "\n" +
	                        Integer.toString(matchesLeftAfterRobotPlay) + 
	                        " left\nRobot wins!");
				}
			}
		});
		
		btnPlayCompleted.setBounds(65, 180, 142, 25);
		getContentPane().add(btnPlayCompleted);
	}
}
