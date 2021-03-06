/*
 * examples/fields/ex-fields-archetype.C
 *
 * Copyright (C) 2002, 2010 G. Villard
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
 */

/** \file examples/fields/ex-fields-archetype.C
 * \author Gilles Villard
 * \brief On using the field archetype to avoid code bloat.
 *
 * Use of a function compiled with the field archetype but called
 * with two distinct field types.
 */

// ---------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
// ---------------------------------------------



//#include "linbox/field/unparametric.h"
//#include "linbox/modular.h"
#include "linbox/field/ntl.h"

#include "linbox/field/archetype.h"

using namespace LinBox;

// ---------------------------------------------
/*  The template function "fct" reads two elements "a" and "b" of the
 *  field "K" from the standard input and writes a/b on the standard output */

template <class Field>
int fct(const Field& K) {

	typedef typename Field::Element K_elt;

	K_elt a,b,r;

	K.init(a); K.init(b); K.init(r);
	K.read(std::cin,a);
	K.read(std::cin,b);
	K.div(r,a,b);
	K.write(std::cout,r) << "\n";
	return 0;
}

// ---------------------------------------------

/// no command line args
int main() {

	/* The field objects "K_o" and "Q_o" are constructed as in previous examples
	*/

	// Givaro::ZRing<NTL::RR> Q_o;
	NTL_RR Q_o ;
	NTL::RR::SetPrecision(400);
	NTL::RR::SetOutputPrecision(50);

	// Givaro::ZRing<NTL::zz_p> K_o;
	NTL::zz_p::init(553);
	NTL_zz_p K_o ;

	/* These field objects "K_o" and "Q_o" of different types can be converted to
	 * objects Q and K of a unique type "Field_archetype" for instance using
	 * a constructor: */


	FieldArchetype Q( & Q_o );
	FieldArchetype K( & K_o );

	/* The template function "fct" is called with two different fields but the
	 * template is instantiated only once since it is called with a unique
	 * template parameter "Field_archetype" */

	fct(Q);
	fct(K);

	return 0;
};

// Local Variables:
// mode: C++
// tab-width: 4
// indent-tabs-mode: nil
// c-basic-offset: 4
// End:
// vim:sts=4:sw=4:ts=4:et:sr:cino=>s,f0,{0,g0,(0,\:0,t0,+0,=s
