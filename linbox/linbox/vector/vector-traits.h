/* -*- mode: c; style: linux -*- */

/* linbox/vector/vector-traits.h
 * Copyright (C) 1999-2001 William J Turner,
 *               2001 Bradford Hovinen
 *
 * Written by William J Turner <wjturner@math.ncsu.edu>,
 *            Bradford Hovinen <hovinen@cis.udel.edu>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef __VECTOR_TRAITS_H
#define __VECTOR_TRAITS_H

#include <vector>	// STL vectors
#include <list>		// STL lists
#include <deque>	// STL deques
#include <utility>      // STL pairs
#include <functional>   // STL functions
#include <map>          // STL maps

/** @name Vector traits.
 * Vector traits are use to allow template specialization to choose different
 * code for dense and sparse vectors.
 */
//@{

// Namespace in which all LinBox library code resides
namespace LinBox
{
	/** List of vector categories.
	 * This structure contains three structures: one relating to dense vectors,
	 * one relating to sparse vectors implemented as sequences of pairs, and 
	 * one relating to sparse vectors implemented as associative containers.
	 * These types allow us to use template specialization to use different 
	 * code for different types of vectors.
	 */
	struct VectorCategories
	{
		struct DenseVectorTag {};
		struct SparseSequenceVectorTag {};
		struct SparseAssociativeVectorTag {};

	}; // struct VectorCategories

	/** Vector traits template structure.
	 * By default, it tries to take all information from the vector class,
	 * but it cannot usually do this.  For example, the vector_category
	 * type is not defined in STL types, so this must be done through 
	 * template specialization.
	 * @param Vector \Ref{LinBox} dense or sparse vector.
	 */
	template <class Vector> struct VectorTraits
	{
		typedef typename Vector::VectorCategory VectorCategory;

		// These are defined for all STL vectors and sequence containers.

	};

	// Specialization for STL vectors
	template <class Element>
	struct VectorTraits< std::vector<Element> >
	{ typedef typename VectorCategories::DenseVectorTag VectorCategory; };

	// Specialization for STL vectors of pairs of size_t and elements
	template <class Element> 
	struct VectorTraits< std::vector< std::pair<size_t, Element> > >
	{ typedef typename VectorCategories::SparseSequenceVectorTag VectorCategory; };

	// Specialization for STL lists of pairs of size_t and elements
	template <class Element> 
	struct VectorTraits< std::list< std::pair<size_t, Element> > >
	{ typedef typename VectorCategories::SparseSequenceVectorTag VectorCategory; };

	// Specialization for STL singly linked lists of pairs of size_t and elements
	template <class Element> 
	struct VectorTraits< std::deque< std::pair<size_t, Element> > >
	{ typedef typename VectorCategories::SparseSequenceVectorTag VectorCategory; };
  
	// Specialization for STL maps of size_t and elements
	template <class Element> 
	struct VectorTraits< std::map<size_t, Element> >
	{ typedef typename VectorCategories::SparseAssociativeVectorTag VectorCategory; };

	// Namespace containing some useful generic functions

	namespace VectorWrapper 
	{
		template <class Field, class Vector>
		inline typename Field::Element &refSpecialized
			(Vector &v, size_t i, VectorCategories::DenseVectorTag tag)
			{ return v[i]; }

		template <class Field, class Vector>
		inline typename Field::Element &refSpecialized
			(Vector &v, size_t i, VectorCategories::SparseSequenceVectorTag tag)
		{
			typename Vector::iterator j;
			typename Field::Element zero;

			for (j = v.begin (); j != v.end () && (*j).first < i; j++);

			if (j == v.end () || (*j).first > i) {
				F.init (zero, 0);
				v.insert (j, pair<size_t, typename Field::Element> (i, zero));
				--j;
			}

			return (*j).second;
		}

		template <class Field, class Vector>
		inline typename Field::Element &refSpecialized
			(Vector &v, size_t i, VectorCategories::SparseAssociativeVectorTag tag)
			{ return v[i]; }

		template <class Field, class Vector>
		inline typename Field::Element &ref (Vector &v, size_t i) 
			{ return refSpecialized<Field, Vector> (v, i, VectorTraits<Vector>::VectorCategory()); }

		template <class Field, class Vector>
		inline const typename Field::Element &constRefSpecialized
			(Vector &v, size_t i, VectorCategories::DenseVectorTag tag)
			{ return v[i]; }

		template <class Field, class Vector>
		inline const typename Field::Element &constRefSpecialized
			(Vector &v, size_t i, VectorCategories::SparseSequenceVectorTag tag)
		{
			typename Vector::iterator j;
			static typename Field::Element zero;

			for (j = v.begin (); j != v.end () && (*j).first < i; j++);

			if (j == v.end () || (*j).first > i) {
				F.init (zero, 0);
				return zero;
			}
			else
				return (*j).second;
		}

		template <class Field, class Vector>
		inline const typename Field::Element &constRefSpecialized
			(Vector &v, size_t i, VectorCategories::SparseAssociativeVectorTag tag)
			{ return v[i]; }

		template <class Field, class Vector>
		inline const typename Field::Element &constRef (Vector &v, size_t i) 
			{ return constRefSpecialized<Field, Vector> (v, i, VectorTraits<Vector>::VectorCategory()); }

		template <class Vector>
		inline void ensureDimSpecialized (Vector &v, size_t n, VectorCategories::DenseVectorTag tag)
			{ v.resize (n); }

		template <class Vector>
		inline void ensureDimSpecialized (Vector &v, size_t n, VectorCategories::SparseSequenceVectorTag tag)
			{}

		template <class Vector>
		inline void ensureDimSpecialized (Vector &v, size_t n, VectorCategories::SparseAssociativeVectorTag tag)
			{}

		template <class Vector>
		inline void ensureDim (Vector &v, size_t n) 
			{ ensureDimSpecialized (v, n, VectorTraits<Vector>::VectorCategory()); }
	}

} // namespace LinBox

//@} Vector traits
#endif // __VECTOR_TRAITS_H
