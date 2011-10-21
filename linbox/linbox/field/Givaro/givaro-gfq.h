/* -*- mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
// vim:sts=8:sw=8:ts=8:noet:sr:cino=>s,f0,{0,g0,(0,\:0,t0,+0,=s
/* linbox/field/givaro-gfq.h
 * Copyright (C) 2002 Pascal Giorgi
 *
 * Written by Pascal Giorgi <pascal.giorgi@ens-lyon.fr>
 * JGD 12.06.2002 : -- I don't see the need of *(new in convert
 * JGD 19.09.2003 : added isZero
 * WJT 24.06.2005 : Removed using declarations
 *
 * ------------------------------------
 *
 * See COPYING for license information.
 */

/*! @file field/Givaro/givaro-gfq.h
 *
 * @warning This wrapper works only with an improved version of Givaro ;
 * This version of givaro won't be available for public yet ;
 * But it is available on my web page ;
 * You can send me a mail to get it or for others details.
 */

#ifndef __LINBOX_field_givaro_gfq_H
#define __LINBOX_field_givaro_gfq_H


#include "linbox/integer.h"
#include "linbox/field/field-traits.h"
#include "linbox/field/field-interface.h"
#include "linbox/util/debug.h"
#include "linbox/field/hom.h"
#include "linbox/linbox-config.h"


//------------------------------------
// Files of Givaro library


#include <givaro/givgfq.h>
#include <givaro/giv_randiter.h>
#include <givaro/givpoly1factor.h>
//------------------------------------

// Namespace in which all LinBox code resides
namespace LinBox
{

	template <class Ring>
	struct ClassifyRing;

	class GivaroGfq;

	template<>
	struct ClassifyRing<GivaroGfq> {
		typedef RingCategories::ModularTag categoryTag;
	};


	class GivaroGfq;

	template<>
	inline integer& FieldTraits<GivaroGfq>::maxModulus( integer& i )
	{
		return i = integer( 32749 );  // prevprime( 2^15 )
	}

	template<>
	inline bool FieldTraits<GivaroGfq>::goodModulus( const integer& i )
	{
		integer max;
		if( i < 2 || i > FieldTraits<GivaroGfq>::maxModulus(max) )
			return false;
		return probab_prime( i, 10 );
	}

	template<>
	inline integer& FieldTraits<GivaroGfq>::maxExponent( integer& i )
	{
		return i = 20;  // Cardinality must be <= 2^20
	}


	/** Wrapper of Givaro's GFqDom<int32_t>  class.
	  \ingroup field

	 *  This class allows to construct only extension fields with a prime characteristic.
	 */
	class GivaroGfq : public Givaro::GFqDom<int32_t>, public FieldInterface {

	public:

		typedef Givaro::GFqDom<int32_t> Father_t ;

		using Father_t::one ;
		using Father_t::zero;
		using Father_t::mOne;
		/** Element type.
		 *  This type is inherited from the Givaro class GFqDom<int32_t>
		 */
		typedef  Givaro::GFqDom<int32_t>::Rep Element;

		/** RandIter type
		 *  This type is inherited from the Givaro class GFqDom<TAG>
		 */
		typedef Givaro::GIV_randIter< Givaro::GFqDom<int32_t>, LinBox::integer >  RandIter;

		/** Empty Constructor
		*/
		GivaroGfq() :
		 Givaro::GFqDom<int32_t>()
		{
			// Givaro::GFqDom<int32_t>::init(one,1);
			// Givaro::GFqDom<int32_t>::init(mOne,-1);
			// Givaro::GFqDom<int32_t>::init(zero,0);
		}

		/** Constructor from an integer
		 *  this constructor use the ZpzDom<TAG> constructor
		 */
		GivaroGfq(const integer& p, const integer& k=1) :
		 Givaro::GFqDom<int32_t>(static_cast<UTT>(int32_t(p)), static_cast<UTT>(int32_t(k)))
		{
			//enforce that the cardinality must be <2^16, for givaro-gfq
			int32_t pl=p;
			for(int32_t i=1;i<k;++i) pl*=(int32_t)p;
			if(!FieldTraits<GivaroGfq>::goodModulus(p)) {
				throw PreconditionFailed(__func__,__FILE__,__LINE__,"modulus be between 2 and 2^15 and prime");
			}
			else if(pl>(1<<20))  {
				throw PreconditionFailed(__func__,__FILE__,__LINE__,"cardinality must be < 2^20");
			}
			// Givaro::GFqDom<int32_t>::init(one,1);
			// Givaro::GFqDom<int32_t>::init(mOne,-1);
			// Givaro::GFqDom<int32_t>::init(zero,0);

		}

		// Dan Roche 6-15-04
		// This constructor takes a vector of ints that represent the polynomial
		// to use (for modular arithmetic on the extension field).
		// Mostly copied from givaro/givgfq.inl
		GivaroGfq(const integer& p, const integer& k, const std::vector<integer>& modPoly) :
		 Givaro::GFqDom<int32_t>(static_cast<UTT>(int32_t(p)), static_cast<UTT>(int32_t(k)))
		{
			// Givaro::GFqDom<int32_t>::init(one,1L);
			// Givaro::GFqDom<int32_t>::init(mOne,-1L);
			// Givaro::GFqDom<int32_t>::init(zero,0L);



			//enforce that the cardinality must be <2^16, for givaro-gfq
			int32_t pl=p;
			for(int32_t i=1;i<k;++i) pl*=(int32_t)p;
			if(!FieldTraits<GivaroGfq>::goodModulus(p)) throw PreconditionFailed(__func__,__FILE__,__LINE__,"modulus be between 2 and 2^15 and prime");
			else if(pl>=(1<<16)) throw PreconditionFailed(__func__,__FILE__,__LINE__,"cardinality must be < 2^16");

			if( k < 2 ) throw PreconditionFailed(__func__,__FILE__,__LINE__,"exponent must be >1 if polynomial is specified");

			if(modPoly.size() != (size_t)(k+1)) throw PreconditionFailed(__func__,__FILE__,__LINE__,"Polynomial must be of order k+1");

		 Givaro::GFqDom<int32_t> Zp(p,1);
			typedef Givaro::Poly1FactorDom< Givaro::GFqDom<int32_t>, Givaro::Dense > PolDom;
			PolDom Pdom( Zp );
			PolDom::Element Ft, F, G, H;

		 Givaro::Poly1Dom< Givaro::GFqDom<int32_t>, Givaro::Dense >::Rep tempVector(k+1);
			for( int i = 0; i < k+1; i++ )
				tempVector[i] = modPoly[i] % p;
			Pdom.assign( F, tempVector );

			Pdom.give_prim_root(G,F);
			Pdom.assign(H,G);

			typedef Givaro::Poly1PadicDom< Givaro::GFqDom<int32_t>, Givaro::Dense > PadicDom;
			PadicDom PAD(Pdom);

			PAD.eval(_log2pol[1],H);
			for (UTT i = 2; i < _qm1; ++i) {
				Pdom.mulin(H, G);
				Pdom.modin(H, F);
				PAD.eval(_log2pol[i], H);
			}

			for (UTT i = 0; i < _q; ++i)
				_pol2log[ _log2pol[i] ] = 1;

			UTT a,b,r,P=p;
			for (UTT i = 1; i < _q; ++i) {
				a = _log2pol[i];
				r = a & P;
				if (r == (P - 1))
					b = a - r;
				else
					b = a + 1;
				_plus1[i] = _pol2log[b] - _qm1;
			}

			_plus1[_qm1o2] = 0;

		}

		/** Characteristic.
		 * Return integer representing characteristic of the domain.
		 * Returns a positive integer to all domains with finite characteristic,
		 * and returns 0 to signify a domain of infinite characteristic.
		 * @return integer representing characteristic of the domain.
		 */
		integer& characteristic(integer& c) const
		{
			return c=integer(static_cast<int32_t>( Givaro::GFqDom<int32_t>::characteristic()));
		}

		int32_t characteristic() const
		{
			return static_cast<int32_t>( Givaro::GFqDom<int32_t>::characteristic());
		}

#if (GIVARO_VERSION<30403)
		unsigned long characteristic(unsigned long & c) const
		{
			return c = static_cast<int32_t>( Givaro::GFqDom<int32_t>::characteristic());
		}
#else
		unsigned long characteristic(unsigned long & c) const
		{
			return Givaro::GFqDom<int32_t>::characteristic(c);
		}
#endif

		/** Cardinality.
		 * Return integer representing cardinality of the domain.
		 * Returns a non-negative integer for all domains with finite
		 * cardinality, and returns -1 to signify a domain of infinite
		 * cardinality.
		 * @return integer representing cardinality of the domain
		 */
		integer& cardinality(integer& c) const
		{
			return c=integer(static_cast<int32_t>( Givaro::GFqDom<int32_t>::size()));
		}


		integer cardinality() const
		{
			return integer(static_cast<int32_t>( Givaro::GFqDom<int32_t>::cardinality()));
		}


		/** Initialization of field base Element from an integer.
		 * Behaves like C++ allocator construct.
		 * This function assumes the output field base Element x has already been
		 * constructed, but that it is not already initialized.
		 * We assume that the type of Element is short int.
		 * this methos is just a simple cast.
		 * @return reference to field base Element.
		 * @param x field base Element to contain output (reference returned).
		 * @param y integer.
		 */
		Element& init(Element& x , const integer& y = 0) const
		{
			return Givaro::GFqDom<int32_t>::init( x, int32_t(y % (integer) _q));
		}

		// TO BE OPTIMIZED
		Element& init(Element& x , const float y) const
		{
			return Givaro::GFqDom<int32_t>::init( x, (double)y);
		}

		template<class YYY>
		Element& init(Element& x , const YYY& y) const
		{
			return Givaro::GFqDom<int32_t>::init( x, y);
		}

		/** Conversion of field base Element to an integer.
		 * This function assumes the output field base Element x has already been
		 * constructed, but that it is not already initialized.
		 * @return reference to an integer.
		 * @param x integer to contain output (reference returned).
		 * @param y constant field base Element.
		 */
		integer& convert(integer& x, const Element& y) const
		{
			int32_t tmp;
			return x = integer( Givaro::GFqDom<int32_t>::convert(tmp,y));
		}
		// TO BE OPTIMIZED
		float& convert(float& x, const Element& y) const
		{
			double tmp;
		 Givaro::GFqDom<int32_t>::convert( tmp, y);
			return x = (float)tmp;
		}

		template<class XXX>
		XXX& convert(XXX& x, const Element& y) const
		{
			return Givaro::GFqDom<int32_t>::convert( x, y);
		}

		//bool isZero(const Element& x) const { return Givaro::GFqDom<int32_t>::isZero(x); }


	}; // class GivaroGfq


	template<>
	class Hom <GivaroGfq,GivaroGfq> {
	public:
		typedef GivaroGfq Source;
		typedef GivaroGfq Target;

		typedef Source::Element SrcElt;
		typedef Target::Element Elt;

		//Hom(){}
		/**
		 * Construct a homomorphism from a specific source ring S and target
		 * field T with Hom(S, T).
		 * Specializations define all actual homomorphisms.
		 */
		Hom(const Source& S, const Target& T) :
			_source(S), _target(T)
		{ }

		/**
		 * image(t, s) implements the homomorphism, assigning the
		 * t the value of the image of s under the mapping.
		 *
		 * The default behaviour goes through integers.
		 */
		Elt& image(Elt& t, const SrcElt& s)
		{
			return _target.init(t, _source.convert(tmp,s));
		}

		/** If possible, preimage(s,t) assigns a value to s such that
		 * the image of s is t.  Otherwise behaviour is unspecified.
		 * An error may be thrown, a conventional value may be set, or
		 * an arb value set.
		 *
		 * The default behaviour goes through integers.
		 */
		SrcElt& preimage(SrcElt& s, const Elt& t)
		{
			return _source.init(s, _target.convert(tmp,t));
		}
		const Source& source() { return _source;}
		const Target& target() { return _target;}

	private:
		integer tmp;
		Source _source;
		Target _target;
	}; // end Hom



} // namespace LinBox

#endif // __LINBOX_field_givaro_gfq_H

