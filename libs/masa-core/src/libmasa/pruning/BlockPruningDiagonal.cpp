/*******************************************************************************
 *
 * Copyright (c) 2010-2015   Edans Sandes
 *
 * This file is part of MASA-Core.
 * 
 * MASA-Core is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * MASA-Core is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with MASA-Core.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#include "BlockPruningDiagonal.hpp"

#include <stdio.h>
#include "../../common/Common.hpp"

//#define DEBUGM (1)

extern int splitstep;
extern FILE * dbbpd;


BlockPruningDiagonal::BlockPruningDiagonal() {
}

BlockPruningDiagonal::~BlockPruningDiagonal()
{
	// TODO Auto-generated destructor stub
}

//void BlockPruningDiagonal::setBlockHeight(int blockHeight) {
//	this->blockHeight = blockHeight;
//	this->blockCountVertical = (seq0_len+blockHeight-1)/blockHeight;
//	this->blockSplitVertical = NULL;
//}
//
//void BlockPruningDiagonal::setBlockWidth(int blockWidth) {
//	this->blockWidth = blockWidth;
//	this->blockCountHorizontal = (seq1_len+blockWidth-1)/blockWidth;
//	this->blockSplitHorizontal = NULL;
//}
//
//
//void BlockPruningDiagonal::setBlockSplitVertical(int count, const int* splits) {
//	this->blockHeight = -1;
//	this->blockCountVertical = count;
//	this->blockSplitVertical = splits;
//}
//
//void BlockPruningDiagonal::setBlockSplitHorizontal(int count, const int* splits) {
//	this->blockWidth = -1;
//	this->blockCountHorizontal = count;
//	this->blockSplitHorizontal = splits;
//}


void BlockPruningDiagonal::getNonPrunableWindow(int* start, int* end) {
	*start = windowStart;
	*end = windowEnd;
}

void BlockPruningDiagonal::initialize() {
	this->windowStart = 0;
	this->windowEnd = getGrid()->getGridWidth();
}

void BlockPruningDiagonal::finalize() {
}

//void BlockPruningDiagonal::getBlockPosition(int bx, int by, int* row, int* column) {
//	if (bx < 0 || by < 0 || bx >= blockCountHorizontal || by >= blockCountVertical) {
//		*column = -1;
//		*row = -1;
//	} else {
//		if (blockSplitHorizontal != NULL) {
//			*column = blockSplitHorizontal[bx];
//		} else {
//			*column = bx*blockWidth;
//		}
//
//		if (blockSplitVertical != NULL) {
//			*row = blockSplitVertical[by];
//		} else {
//			*row = by*blockHeight;
//		}
//	}
//}

//int BlockPruningDiagonal::isBlockPrunable(const int bx, const int by, score_t block_score, int best_score) {
//	int far_i;
//	int far_j;
//	getBlockPosition(bx, by, &far_i, &far_j);
//	if (far_i == -1) {
//		return true;
//	}
//
//	int distI = seq0_len-far_i;
//	int distJ = seq1_len-far_j;
//
//	int dist = (distI<distJ)?distI:distJ;
//	int max = block_score.score + dist*score_params->match;
//	//fprintf(stderr, "isBlockPrunable(%d,%d, [%d], %d) - Dist: %d - Max: %d \n", bx, by, block_score.score, best_score, dist, max);
//	return (max < best_score);
//}

void BlockPruningDiagonal::updatePruningWindow(int diagonal, const score_t* block_scores) {
	//printf("\n Diag: %d - start: %d - end: %d - width: %d", diagonal, windowStart, windowEnd, getGrid()->getGridHeight());
        for (int i=windowStart; i<windowEnd; i++) {
		updateBestScore(block_scores[i].score);
	}


        if  (diagonal > getGrid()->getGridHeight()/2) {
		// ''While'' loop is ONLY safe if the blocks are parallelograms.
		// If the blocks are rectangle, use an "if" clause.
		while (windowStart < getGrid()->getGridWidth()) {
			int bx = windowStart;
			int by = diagonal-bx;

			//if (getGrid()->split) {
			//	if (!isBlockPrunableMulti(bx, by, block_scores[bx].score)) {
			//		break;
			//    }
			//}
			//else {
				if (!isBlockPrunable(bx, by, block_scores[bx].score)) {
					break;
				}
                               else {

if (DEBUGM) fprintf(dbbpd, "111 ** Diag: %d - start: %d - end: %d - width: %d - heigth: %d ** bx: %d - by: %d - score: %d \n", diagonal, windowStart, windowEnd,getGrid()->getGridWidth(), getGrid()->getGridHeight(), bx, by, block_scores[bx].score);
 

                               }
			windowStart++;
		}
	} 

	// The right-pruning for rectangular blocks must be redesigned, since we
	// need two diagonals to certify that a row is totally prunable.
	if (windowEnd < windowStart) {
		windowEnd = windowStart-1; // It must be strict less (TODO confirmar)
	} else if (diagonal > getGrid()->getGridWidth()) {
		bool isLastBlockPrunable;
                isLastBlockPrunable = isBlockPrunable(windowEnd, diagonal-windowEnd, block_scores[windowEnd].score);
//                else
//                   isLastBlockPrunable = isBlockPrunable(windowEnd, diagonal-windowEnd, 0);
		if (!isLastBlockPrunable) {
			windowEnd++;
		} else { 
if (DEBUGM) fprintf(dbbpd,"222 ** Diag: %d - start: %d - end: %d - width: %d - height: %d ** bx: %d - by: %d - score: %d \n", diagonal, windowStart, windowEnd, getGrid()->getGridWidth(), getGrid()->getGridHeight(), windowEnd,(diagonal-windowEnd) , block_scores[windowEnd].score);



			int previousEnd = windowEnd;
			while (windowEnd > windowStart) {
				int bx = windowEnd-1;
				int by = diagonal-bx;
				if (!isBlockPrunable(bx, by, block_scores[bx].score)) {
						break;
				}
                                else {
if (DEBUGM) fprintf(dbbpd, "333 ** Diag: %d - start: %d - end: %d - width: %d - height: %d  **  bx: %d - by: %d - score: %d \n", diagonal, windowStart, windowEnd,getGrid()->getGridWidth(), getGrid()->getGridHeight(), windowEnd,(diagonal-windowEnd) , block_scores[windowEnd].score);


                                }
				windowEnd--;
			}
		}
	}
}



void BlockPruningDiagonal::updatePruningWindowMulti(int diagonal, const score_t* block_scores) {
        int bx = 0;
        int by = 1;
        //printf("\n Diag: %d - start: %d - end: %d", diagonal, windowStart, windowEnd);
        windowStart = 0;
         
        //int diag = diagonal - (splitstep-1)*getGrid()->getGridWidth();
        for (int i=windowStart; i<windowEnd; i++) {
                updateBestScore(block_scores[i].score);
        }

        
        if  (diagonal >  ((getGrid()->getGridHeight()/2) - (splitstep-1)*getGrid()->getGridWidth())) {
                // ''While'' loop is ONLY safe if the blocks are parallelograms.
                // If the blocks are rectangle, use an "if" clause.
                //printf ("*** \n");
                bool isLastBlockPrunable;
                isLastBlockPrunable = isBlockPrunableMulti(windowStart, diagonal-windowStart, block_scores[windowStart].score,diagonal);
                
                //if ((!isLastBlockPrunable) && (windowStart > 0)) {
                //        windowStart--;
                //} else {

                   //while ((windowStart < getGrid()->getGridWidth()) && (windowStart < windowEnd)) {
                   while (windowStart < windowEnd) {
                        bx = windowStart;
                        by = diagonal-bx;

                        //printf("*** 1: Diag: %d - bx: %d - by: %d *** \n", diagonal, bx, by);
                        if (!isBlockPrunableMulti(bx, by, block_scores[bx].score, diagonal)) {
                           break;
                        }
                        else {
//if (DEBUGM) fprintf(dbbpd, "111 ** Diag: %d - start: %d - end: %d - width: %d - heigth: %d ** bx: %d - by: %d - score: %d \n", diagonal, windowStart, windowEnd,getGrid()->getGridWidth(), getGrid()->getGridHeight(), bx, by, block_scores[bx].score);
                        }
                        windowStart++;
                   }
              //}
        } 
        
       // The right-pruning for rectangular blocks must be redesigned, since we
        // need two diagonals to certify that a row is totally prunable.
        if (windowEnd < windowStart) {
                windowEnd = windowStart-1; // It must be strict less (TODO confirmar)
        } else if (diagonal > getGrid()->getGridWidth()) {
                bool isLastBlockPrunable;
                isLastBlockPrunable = isBlockPrunableMulti(windowEnd, diagonal-windowEnd, block_scores[windowEnd].score,diagonal);
                
                if (!isLastBlockPrunable) {
                        windowEnd++;
                } else {
//if (DEBUGM) fprintf(dbbpd,"222 ** Diag: %d - start: %d - end: %d - width: %d - height: %d ** bx: %d - by: %d - score: %d \n", diagonal, windowStart, windowEnd, getGrid()->getGridWidth(), getGrid()->getGridHeight(), windowEnd,(diagonal-windowEnd) , sendzero?0:block_scores[windowEnd].score);
                       
                        int previousEnd = windowEnd;
                        while (windowEnd > windowStart) {
                                bx = windowEnd-1;
                                by = diagonal-bx;
                                if (!isBlockPrunableMulti(bx, by, block_scores[bx].score, diagonal)) {
                                   break;
                                }
                                else {
//if (DEBUGM) fprintf(dbbpd, "333 ** Diag: %d - start: %d - end: %d - width: %d - height: %d  **  bx: %d - by: %d - score: %d \n", diagonal, windowStart, windowEnd,getGrid()->getGridWidth(), getGrid()->getGridHeight(), windowEnd,(diagonal-windowEnd) , block_scores[windowEnd].score);
                               }

                               windowEnd--;
                        }
                }
        }

}

