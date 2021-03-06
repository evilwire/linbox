/* Copyright (C) <+years+> the members of the LinBox group
 * Written by <+someone+> < <+her mail+> >
 *
 * This file is part of the LinBox library.
 *
 * ========LICENCE========
 * LinBox is free software: you can redistribute it and/or modify
 * it under the terms of the  GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * LinBox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 * ========LICENCE========
 *
 * other mentions
 */

#ifndef __LINBOX_<+directory_file_name+>_H
#define __LINBOX_<+directory_file_name+>_H

/** @file <+directory/file-name.h+>
 * @brief desc
 * long doxy desc
 * @bib bibliography
 */

#include "<++>"

#if 0 // better than commenting out code with big /*   */
// (it shows this code is not really mature yet...)

#define LB_VAR  10  /* local var */
#define LB_DEF      /* local define */
#define LB_MACRO () /* local macro */

#ifdef _LINBOX_DEF   // linbox global define
#ifdef _LINBOX_MACRO // linbox global macro
#if _LINBOX_VAR // linbox global variable
#endif
#endif
#endif

#endif

namespace LinBox
{
	/**  @brief this function is about...
	 * this important function has comments  !!!
	 */
	template<++>
	void my_func(T & toto)
	{
		toto() ;
		if (a) {
			b();
		}
		else {
			c() ;
		}
	}

	template<class A,
	class B>
	void foo() ;

	switch(a) {
	case toto:
		a() ;
		break;
	case titi:
		{
			b() ;
			break;
		}
	default :
		{
			b() ;
		}
	}

	class A {
	private :
		int _p ;
		int _q ;
	public :
		A() :
			_p(0), _q(0)
		{} ;

		A(int q) : _p(1), _q(q)
		{} ;

		A(int p, int q)
		: _p(p), _q(q)
		{} ;


		~A() {} ;
	};
#if 0
	int old_code()  // but maybe usefull later
	{
		prinf("comment out code !");
	}
#endif

} //LinBox

#include "<+file-name.inl+>" // implementation here

#undef LB_VAR    // environmentalists love us
#undef LB_DEF    // really !
#undef LB_MACRO

#endif //__LINBOX_<+directory_file_name+>_H

// Local Variables:
// mode: C++
// tab-width: 4
// indent-tabs-mode: nil
// c-basic-offset: 4
// End:
// vim:sts=4:sw=4:ts=4:et:sr:cino=>s,f0,{0,g0,(0,\:0,t0,+0,=s
