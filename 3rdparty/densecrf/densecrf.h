/*
    Copyright (c) 2011, Philipp Krähenbühl
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
        * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
        * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
        * Neither the name of the Stanford University nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY Philipp Krähenbühl ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL Philipp Krähenbühl BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include "types.h"
#include "permutohedral.h"
#include "pairwisepotential.h"

class PairwisePotential;

class DenseCRF {
public:
	DllExport DenseCRF(int nNodes, byte nStates);
	DllExport virtual ~DenseCRF();
	// Add  a pairwise potential defined over some feature space
	// The potential will have the form:    w*exp(-0.5*|f_i - f_j|^2)
	// The kernel shape should be captured by transforming the
	// features before passing them into this function
	DllExport void addPairwiseEnergy(const float *features, int D, float w=1.0f, const SemiMetricFunction * function = NULL);
	
	// Add your own favorite pairwise potential (ownwership will be transfered to this class)
	DllExport void addPairwiseEnergy( PairwisePotential* potential );
	
	// Set the unary potential for all variables and labels (memory order is [x0l0 x0l1 x0l2 .. x1l0 x1l1 ...])
	DllExport void setNodes(const Mat &pots);
	
	// Run inference and return the probabilities
	DllExport void infer(unsigned int nIt = 1, float *result = NULL, float relax = 1.0f);
	
	// Run MAP inference and return the map for each pixel
	DllExport vec_byte_t decode(unsigned int nIt = 0, float relax = 1.0);
	
	// Step by step inference
	DllExport void startInference();
	DllExport void stepInference( float relax = 1.0 );
	DllExport void currentMap( short * result );
	
public: /* Debugging functions */
	// Compute the unary energy of an assignment
	DllExport void unaryEnergy( const short * ass, float * result );
	
	// Compute the pairwise energy of an assignment (half of each pairwise potential is added to each of it's endpoints)
	DllExport void pairwiseEnergy( const short * ass, float * result, int term=-1 );


protected:
	friend class BipartiteDenseCRF;

	int		m_nNodes;		// number of pixels
	byte	m_nStates;
	
	float *unary_, *additional_unary_, *current_, *next_, *tmp_;

	// Store all pairwise potentials
	std::vector<PairwisePotential *> pairwise_;

	// Run inference and return the pointer to the result
	float *runInference(unsigned int nIt, float relax);

	// Auxillary functions
	void expAndNormalize(float* out, const float* in, float scale = 1.0, float relax = 1.0);

	// Don't copy this object, bad stuff will happen
	DenseCRF(DenseCRF & o) {}


private:

};


class DenseCRF2D : public DenseCRF {
public:
	DllExport DenseCRF2D(int width, int height, byte nStates) : DenseCRF(width * height, nStates), m_width(width), m_height(height) {}
	DllExport virtual ~DenseCRF2D(void) {}
	
	// Add a Gaussian pairwise potential with standard deviation sx and sy
	DllExport void addPairwiseGaussian( float sx, float sy, float w, const SemiMetricFunction * function = NULL );
	
	// Add a Bilateral pairwise potential with spacial standard deviations sx, sy and color standard deviations sr,sg,sb
	DllExport void addPairwiseBilateral( float sx, float sy, float sr, float sg, float sb, const unsigned char * im, float w, const SemiMetricFunction * function = NULL );
	

private:
	int m_width;		// TODO: maybe CvSize?
	int m_height;
};
