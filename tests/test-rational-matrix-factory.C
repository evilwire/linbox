/* tests/test-det.C
 * Copyright (C) 2002 Bradford Hovinen
 *
 * Written by Bradford Hovinen <hovinen@cis.udel.edu>
 *
 * --------------------------------------------------------
 *
 * 
 * ========LICENCE========
 * This file is part of the library LinBox.
 * 
 * LinBox is free software: you can redistribute it and/or modify
 * it under the terms of the  GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 * ========LICENCE========
 *
 */

/*! @file  tests/test-rational-matrix-factory.C
 * @ingroup tests
 * @brief no doc.
 * @test no doc.
 */


#include <linbox/linbox-config.h>

#include <iostream>
#include <fstream>

#include <cstdio>

#include "linbox/linbox-config.h"

#include "linbox/util/commentator.h"
#include "givaro/zring.h"
#include "linbox/field/gmp-rational.h"
#include "linbox/matrix/dense-matrix.h"
#include "linbox/blackbox/rational-matrix-factory.h"

#include "test-common.h"

using namespace LinBox;

/* Test : For a diagonal rational matrix A = diag(1,1/2,1/3,...) compute
 * -> rational norm |A|_r = n
 * -> number of non-zero elements = n
 * -> number of truly rational elements = n-1
 * -> denA - the common denominator of A = lcm(1,...,n)
 * -> d[i] - the common denominator for ith row = i
 * -> Atilde = Id
 * -> Aprim[j,j] = denA / j
 *
 * n - Dimension to which to make matrix
 *
 * Return true on success and false on failure
 */

static bool testDiagonalMatrix (size_t n)
{
	commentator().start ("Testing rational matrix factory for dense matrix", "testRationalMatrixFactory");

	bool ret = true;
	size_t j;

	GMPRationalField Q;
	BlasMatrix<GMPRationalField > A(Q,n,n);
	integer lcm_n=1;

	for (j = 0; j < n; j++) {
		GMPRationalField::Element tmp;
		Q.init(tmp, 1,j+1);
		A.setEntry(j,j,tmp);
		//Q.init(A.refEntry(j,j),1,j+1);
		lcm(lcm_n,lcm_n,j+1);
	}

	RationalMatrixFactory<Givaro::ZRing<Integer>, GMPRationalField, BlasMatrix<GMPRationalField > > FA(&A);

	integer ratnorm,aprimnorm,atildenorm;
	FA.getNorms(ratnorm,aprimnorm,atildenorm);

	ostream &report = commentator().report (Commentator::LEVEL_IMPORTANT, INTERNAL_DESCRIPTION);

	report << "True rational norm: " << n << endl;
	report << "Computed rational norm: " << ratnorm << endl;

	report << "True norm of A': " << lcm_n << endl;
        report << "Computed norm of A': " << aprimnorm << endl;

	report << "True norm of Atilde: " << 1 << endl;
        report << "Computed norm of Atilde: " << atildenorm << endl;

	if ( (ratnorm != (Integer)n) || ( aprimnorm != lcm_n) || (atildenorm != 1) ) {
		ret = false;
		commentator().report (Commentator::LEVEL_IMPORTANT, INTERNAL_ERROR)
			<< "ERROR: Norms are incorrect" << endl;
	}

	size_t omega, rat_omega;
	FA.getOmega(omega,rat_omega);

	report << "True Omega: " << n  << endl;
	report << "Computed Omega: " << omega  << endl;

        report << "True Rational Omega: " << n-1  << endl;
        report << "Computed Rational Omega: " << rat_omega  << endl;

	if ( (omega != n) || (rat_omega != n-1) ) {
		ret = false;
		commentator().report (Commentator::LEVEL_IMPORTANT, INTERNAL_ERROR)
			 << "ERROR: Number of rational/non-zero elements is incorrect" << endl;
	}

	integer d;
	FA.denominator(d);

	report << "True common denominator: " << lcm_n << endl;
	report << "Computed common denominator: " << d << endl;

	if (d != lcm_n) {
		ret = false;
		commentator().report (Commentator::LEVEL_IMPORTANT, INTERNAL_ERROR)
			<< "ERROR: Common denominator is incorrect" << endl;
	}

	for (j=0; j < n; ++j) {
		FA.denominator(d,(int)j);
		report << "True common denominator for " << j+1 <<  "th row: " << j+1 << endl;
		report << "Computed common denominator for " << j+1 <<  "th row: " << d << endl;
		if (d != (integer)(j+1)) {
			ret  = false;
			commentator().report (Commentator::LEVEL_IMPORTANT, INTERNAL_ERROR)
				<< "ERROR: Common denominator for " << j+1 <<  "th row is incorrect" << endl;
		}
	}

	Givaro::ZRing<Integer> Z;
	BlasMatrix<Givaro::ZRing<Integer> > Aprim(Z,n,n);
	BlasMatrix<Givaro::ZRing<Integer> > Atilde(Z,n,n);

	FA.makeAprim(Aprim);
	FA.makeAtilde(Atilde);

	Aprim.write(report);
	Atilde.write(report);

	for (j=0; j <n; ++j) {
		if (Aprim.getEntry(j,j) != lcm_n/(j+1)) {
			ret  = false;
			commentator().report (Commentator::LEVEL_IMPORTANT, INTERNAL_ERROR)
				<< "ERROR: Aprim is bad at " << j+1 <<  "diagonal entry" << endl;
		}
		if (Atilde.getEntry(j,j) != 1) {
			ret  = false;
			commentator().report (Commentator::LEVEL_IMPORTANT, INTERNAL_ERROR)
				<< "ERROR: Atilde is bad at " << j+1 <<  "diagonal entry" << endl;
		}
	}

	commentator().stop (MSG_STATUS (ret), (const char *) 0, "testRationalMatrixFactory");

	return ret;
}

int main (int argc, char **argv)
{
	bool pass = true;

	static size_t n = 10;
	//static integer q = 4093U;
	//static int iterations = 2;

	static Argument args[] = {
		{ 'n', "-n N", "Set dimension of test matrices to NxN", TYPE_INT,     &n },
		//{ 'q', "-q Q", "Operate over the \"field\" GF(Q) [1]", TYPE_INTEGER, &q },
		//{ 'i', "-i I", "Perform each test for I iterations",    TYPE_INT,     &iterations },
		END_OF_ARGUMENTS
	};

	parseArguments (argc, argv, args);

	commentator().start("Rational Matrix Factory test suite", "rmf");

	// Make sure some more detailed messages get printed
	commentator().getMessageClass (INTERNAL_DESCRIPTION).setMaxDepth (3);
	commentator().getMessageClass (INTERNAL_DESCRIPTION).setMaxDetailLevel (Commentator::LEVEL_UNIMPORTANT);

	if (!testDiagonalMatrix( n )) pass = false;

	commentator().stop("Rational Matrix Factory test suite");
	return pass ? 0 : -1;
}

// Local Variables:
// mode: C++
// tab-width: 4
// indent-tabs-mode: nil
// c-basic-offset: 4
// End:
// vim:sts=4:sw=4:ts=4:et:sr:cino=>s,f0,{0,g0,(0,\:0,t0,+0,=s
