public class MatchesImpl {
	
	private int m_numMatches = 0;
	
	public MatchesImpl()
	{
		// None needed
	}
	
	public void setUserChoice(int choice)
	{
		m_numMatches -= choice;
	}
	
	boolean gameOver()
	{
		return m_numMatches == 0;
	}
	
	public int robotPlay()
	{
		int robotChoice = m_numMatches % 5;
		if (robotChoice == 0) {
			robotChoice = 1;
		}
		
		m_numMatches -= robotChoice;
		return robotChoice;
	}
	
	public int getNumMatches()
	{
		return m_numMatches;
	}
	
	public void setNumMatches(int initialMatches)
	{
		m_numMatches = initialMatches;
	}

}
