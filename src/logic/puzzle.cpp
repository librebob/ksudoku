/***************************************************************************
 *   Copyright 2007      Francesco Rossi <redsh@email.it>                  *
 *   Copyright 2006-2007 Mick Kappenburg <ksudoku@kappendburg.net>         *
 *   Copyright 2006-2007 Johannes Bergmeier <johannes.bergmeier@gmx.net>   *
 *   Copyright 2015      Ian Wadham <iandw.au@gmail.com>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include "puzzle.h"
#include <cstdlib>

#include <QString>

#include "sudokuboard.h"
#include "mathdokugenerator.h"

namespace ksudoku {

Puzzle::Puzzle(SKGraph *graph, bool withSolution)
	: m_withSolution(withSolution)
	, m_graph(graph)
	, m_difficulty(0)
	, m_symmetry(0)
	, m_initialized(false)
{ }

int Puzzle::value(int index) const {
	return ((index < m_puzzle.size()) ? m_puzzle.at(index) : 0);
}

int Puzzle::solution(int index) const {
	return ((index < m_solution.size()) ? m_solution.at(index) : 0);
}

int Puzzle::hintIndex(int moveNum) const {
	return ((moveNum >= m_hintList.count()) ? -1 : m_hintList.at(moveNum));
}

bool Puzzle::init() {
	if(m_initialized) return false;

	if(m_withSolution)
		return false;

	// Set up an empty puzzle.  The user will enter his/her own puzzle.
	if(m_graph) {
	    m_puzzle = m_graph->emptyBoard();
	}
	return true;
}

bool Puzzle::init(int difficulty, int symmetry) {
	if(m_initialized) return false;

	SudokuBoard * board = new SudokuBoard (m_graph);

	// Generate a puzzle and its solution.
	bool success =  board->generatePuzzle (m_puzzle, m_solution,
			      (Difficulty) difficulty, (Symmetry) symmetry);
	if (success) {
	    board->getMoveList (m_hintList);
	}
	// Too many tries at generating a puzzle that meets the user's reqs.
	else {
	    m_puzzle.clear();		// Wipe the puzzle and its solution.
	    m_solution.clear();
	}
	delete board;
	return success;
}

int Puzzle::init(const BoardContents & values) {
	if(m_initialized) return -1;
	int result = -1;
	SudokuType t = m_graph->specificType();

	// Save the puzzle values and solution (if any).
	m_puzzle = values;
	m_hintList.clear();

	if ((t != Mathdoku) && (t != KillerSudoku)) {
	    SudokuBoard * board = new SudokuBoard (m_graph);
	    m_solution = board->solveBoard (m_puzzle);

	    // Get SudokuBoard to check the solution.
	    result = board->checkPuzzle (m_puzzle);
	    if (result != 0) {
		board->getMoveList (m_hintList);
	    }
	    if (result >= 0) {
		result = 1;		// There is one solution.
	    }
	    else if (result == -1) {
		result = 0;		// There is no solution.
	    }
	    else {
		result = 2;		// There is more than one solution.
	    }
	    delete board;
	}
	else {
	    MathdokuGenerator mg (m_graph);
	    result = mg.solveMathdokuTypes (m_solution, &m_hintList);
	}
	return result;
}

}
