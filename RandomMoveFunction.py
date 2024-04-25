
import random

def make_move(board):
    """
    Selects a random column to make a move in, ensuring the column is not full.
    
    Args:
    board (list of lists): The current state of the Connect-4 board. Each inner list represents a column.
    
    Returns:
    int: The index of the column where the disc will be dropped.
    """
    # Get the indices of columns that are not full (i.e., the top row is not occupied)
    available_columns = [index for index, column in enumerate(board) if column[0] == 0]
    
    # Select a random column from the list of available columns
    if available_columns:
        return random.choice(available_columns)
    else:
        # This else part should ideally never be hit if the game checks for a draw/full board before making moves
        raise ValueError("No available columns left to make a move.")
