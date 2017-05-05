/*
 * Copyright (C) 2017 Matthew Wezowicz
 *
 * Written by :
 *               Matthew Wezowicz <mwezz@udel.edu>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 * ========LICENCE========
 */

/** @file matrix/blockedmatrix/blocked-matrix.h
 * @ingroup blockedmatrix
 * A \c BlockedMatrix<_Field,_Allocator>\c represents a matrix as an array of
 * submatrices.
 *
 */

#ifndef __LINBOX_matrix_blockedmatrix_blocked_matrix_H
#define __LINBOX_matrix_blockedmatrix_blocked_matrix_H

#include "linbox/matrix/blockedmatrix/block.h"

namespace LinBox
{
	/**
	 *
	 */
	template<class _Field, class _Allocator>
	class BlockedMatrix{
	public:
		typedef _Field                                     Field;
		typedef typename Field::Element                  Element;    //!< Element type
		typedef _Allocator                             Allocator;
		typedef typename Allocator::Block_t              Block_t;
		typedef BlockedMatrix<Field,Allocator>            Self_t;    //!< Self type
		typedef const BlockedMatrix<Field,Allocator> constSelf_t;    //!< Self type

                typedef Self_t                                matrixType;    //!< matrix type
                typedef constSelf_t                      constMatrixType;    //!< matrix type
                typedef Self_t                                  blasType;    //!< blas matrix type
	
	protected:
		const Field*  _field;
		size_t         _rows; //!< Number of rows of blocks
		size_t         _cols; //!< Number of columns of blocks
		size_t   _block_rows; //!< Number of rows per block
		size_t   _block_cols; //!< Number of columns per block
		Allocator _allocator;

	public:
		//////////////////
		// CONSTRUCTORS //
		//////////////////

		/** Allocates a new zero block matrix of \f$ M \times N\f$ 
		 * blocks, with each block \f$ m \times n\f$ in dimension.
		 * @param F
		 * @param m rows of blocks
		 * @param n cols of blocks
		 * @param block_m rows per block
		 * @param block_n cols per block
		 *
		 */
		BlockedMatrix(
			const Field& F,
			const size_t m,
			const size_t n,
			const size_t block_m,
			const size_t block_n) :
			// Init list begins here.
			_field(&F),
			_rows(m),
			_cols(n),
			_block_rows(block_m),
			_block_cols(block_n),
		        _allocator(F,m,n,block_m,block_n){

			_allocator.allocate();
		}

		/** Allocates a new zero block matrix of \f$ M \times N\f$ 
		 * blocks, with each block \f$ m \times n\f$ in dimension.
		 * @param F
		 * @param m rows of blocks
		 * @param n cols of blocks
		 * @param block_m rows per block
		 * @param block_n cols per block
		 * @param allocator preconfigured allocator
		 *
		 */
		BlockedMatrix(
			const Field& F,
			const size_t m,
			const size_t n,
			const size_t block_m,
			const size_t block_n,
			const Allocator& allocator) :
			// Init list begins here.
			_field(&F),
			_rows(m),
			_cols(n),
			_block_rows(block_m),
			_block_cols(block_n),
			_allocator(allocator){

			_allocator.allocate();
		}

		/** Allocates a new zero block matrix of \f$ M \times N\f$ 
		 * blocks, with each block \f$ m \times n\f$ in dimension,
		 * and copys the content of the provided block matrix.
		 *
		 * Will not work properly with preconfigured allocators.
		 *
		 * @param rhs block matrix to copy.
		 *
		 */
		BlockedMatrix(const BlockedMatrix& rhs) :
			// Init list begins here.
			_field(rhs._field),
			_rows(rhs._rows),
			_cols(rhs._cols),
			_block_rows(rhs._block_rows),
			_block_cols(rhs._block_cols),
			_allocator(*_field,_rows,_cols,_block_rows,_block_cols){

			_allocator.allocate();

			for(size_t i = 0; i < _rows; i++){
				for(size_t j = 0; j < _cols; j++){
					setBlock(i,j,rhs.getBlock(i,j));
				}
			}
		}

		/**
		 *
		 */
		BlockedMatrix& operator=(const BlockedMatrix& rhs){
			if(this == &rhs){
				return *this;
			}

			bool same = *_field == *(rhs._field);
			same &= _rows == rhs._rows;
			same &= _cols == rhs._cols;
			same &= _block_rows == rhs._block_rows;
			same &= _block_cols == rhs._block_cols;

			if(!same){
				_allocator.deallocate();

				_field = rhs._field;
				_rows = rhs._rows;
				_cols = rhs._cols;
				_block_rows = rhs._block_rows;
				_block_cols = rhs._block_cols;

				_allocator = Allocator(*_field,
						       _rows,
						       _cols,
						       _block_rows,
						       _block_cols);
			}

			for(size_t i = 0; i < _rows; i++){
				for(size_t j = 0; j < _cols; j++){
					setBlock(i,j,rhs.getBlock(i,j));
				}
			}
		}

		/**
		 *
		 */
		~BlockedMatrix(){
			_allocator.deallocate();
		}

		//////////////////
		//  DIMENSIONS  //
		//////////////////

		/** Get the overall number of rows in the matrix.
		 * @returns Overall number of rows in matrix
		 */
		size_t rowdim() const{
			return _rows * _block_rows;
		}

		/** Get the overall number of columns in the matrix.
		 * @returns Overall number of columns in matrix
		 */
		size_t coldim() const{
			return _cols * _block_cols;
		}

		/** Get the number of block rows in the matrix.
		 * @returns Number of block rows in matrix
		 */
		size_t blockRowdim() const{
			return _rows;
		}

		/** Get the number of block columns in the matrix.
		 * @returns Number of block columns in matrix
		 */
		size_t blockColdim() const{
			return _cols;
		}

		/** Get the number of rows per block in the matrix.
		 * @returns Number of rows per block in matrix
		 */
		size_t rowsPerBlock() const{
			return _block_rows;
		}

		/** Get the number of columns per block in the matrix.
		 * @returns Number of columns per block in matrix
		 */
		size_t colsPerBlock() const{
			return _block_cols;
		}

		//////////////////
		//    BLOCKS    //
		//////////////////

		/** Set the block at the (i, j) position to a_ij.
		 * @param i Block Row number, 0...blockRowdim() - 1
		 * @param j Block Column number 0...blockColdim() - 1
		 * @param a_ij Block to set
		 */
		void setBlock(size_t i, size_t j, const Block_t& a_ij){
			_allocator.lookupBlock(i,j) = a_ij;
		}

		/** Get a writeable reference to the block in the (i, j) position.
		 * @param i Block Row index of entry
		 * @param j Block Column index of entry
		 * @returns Reference to block
		 */
		Block_t& refBlock(size_t i, size_t j){
			return _allocator.lookupBlock(i,j);
		}

		/** Get a read-only reference to the block in the (i, j) position.
		 * @param i Block Row index
		 * @param j Block Column index
		 * @returns Const reference to block
		 */
		const Block_t& getBlock(size_t i, size_t j){
			return refBlock(i,j);
		}

		/** Copy the (i, j) block into x, and return a reference to x.
		 * This form is more in the Linbox style and is provided for interface
		 * compatibility with other parts of the library
		 * @param x Block in which to store result
		 * @param i Block Row index
		 * @param j Block Column index
		 * @returns Reference to x
		 */
		Block_t& getBlock(Block_t& x, size_t i, size_t j){
			x = getBlock(i,j);
			return x;
		}

		//////////////////
		//   ELEMENTS   //
		//////////////////

		/** Set the entry at the (i, j) position to a_ij.
		 * @param i Row number, 0...rowdim () - 1
		 * @param j Column number 0...coldim () - 1
		 * @param a_ij Element to set
		 */
		void setEntry(size_t i, size_t j, const Element& a_ij){
			size_t block_i = i / _block_rows;
			size_t block_j = j / _block_cols;
			i = i % _block_rows;
			j = j % _block_cols;
			(*refBlock(block_i,block_j)).setEntry(i,j,a_ij);
		}

		/** Get a writeable reference to the entry in the (i, j) position.
		 * @param i Row index of entry
		 * @param j Column index of entry
		 * @returns Reference to matrix entry
		 */
		Element& refEntry(size_t i, size_t j){
			size_t block_i = i / _block_rows;
			size_t block_j = j / _block_cols;
			i = i % _block_rows;
			j = j % _block_cols;
			return (*refBlock(block_i,block_j)).refEntry(i,j);	
		}

		/** Get a read-only reference to the entry in the (i, j) position.
		 * @param i Row index
		 * @param j Column index
		 * @returns Const reference to matrix entry
		 */
		const Element& getEntry(size_t i, size_t j){
			return refEntry(i,j);
		}

		/** Copy the (i, j) entry into x, and return a reference to x.
		 * This form is more in the Linbox style and is provided for interface
		 * compatibility with other parts of the library
		 * @param x Element in which to store result
		 * @param i Row index
		 * @param j Column index
		 * @returns Reference to x
		 */
		Element& getEntry(Element& x, size_t i, size_t j){
			x = getEntry(i,j);
			return x;
		}

		//////////////////
		//   UTILITIES  //
		//////////////////

		/** Copy all elements of the input matrix into the blocked matrix.
		 * This assumes that the dimensions of the input are less than or
		 * the same as the dimensions of the blocked matrix.
		 * @param m Input matrix
		 * @returns Reference to this blocked matrix
		 */
		template<class _M>
		Self_t& copyFromMatrix(const _M& m){
			size_t rows = m.rowdim();
			size_t cols = m.coldim();

			for(size_t i = 0; i < rows; i++){
				for(size_t j = 0; j < cols; j++){
					setEntry(i,j,m.getEntry(i,j));
				}
			}

			return *this;
		}

		/** Copy all elements of the blocked matrix into the provided matrix.
		 * This assumes that the dimensions of the blocked matrix are
		 * less than or the same as the dimensions of the provided matrix.
		 * @param m A matrix
		 * @returns Reference to this blocked matrix
		 */
		template<class _M>
		Self_t& copyToMatrix(_M& m){
			size_t rows = rowdim();
			size_t cols = coldim();

			for(size_t i = 0; i < rows; i++){
				for(size_t j = 0; j < cols; j++){
					m.setEntry(i,j,getEntry(i,j));
				}
			}

			return *this;
		}

	}; // end of class BlockedMatrix

	/**
	 *
	 */
	template<class _BlockedMatrix>
	class AlignedBlockedSubmatrix{
	public:
		typedef typename _BlockedMatrix::Field                 Field;
		typedef typename Field::Element                      Element;
		typedef typename _BlockedMatrix::Allocator         Allocator;
		typedef typename Allocator::Block_t                  Block_t;	
		typedef AlignedBlockedSubmatrix<_BlockedMatrix>       Self_t;
		typedef const Self_t                             constSelf_t;
		typedef Self_t                                 subMatrixType;
		typedef constSelf_t                       constSubMatrixType;
		typedef typename _BlockedMatrix::Self_t           matrixType;
		typedef typename _BlockedMatrix::constSelf_t constMatrixType;
		typedef matrixType                                  blasType;

	protected:
		size_t         _rows; //!< Number of rows of blocks
		size_t         _cols; //!< Number of columns of blocks
		size_t   _block_rows; //!< Number of rows per block
		size_t   _block_cols; //!< Number of columns per block
		size_t           _r0; //!< Upper left corner block row of AlignedBlockedSubmatrix in \p _Mat
		size_t           _c0; //!< Upper left corner block column of AlignedBlockedSubmatrix in \p _Mat
		_BlockedMatrix& _Mat; //!< Parent BlockedMatrix

	public:	
		//////////////////
		// CONSTRUCTORS //
		//////////////////

		/** Constructor from an existing @ref BlockedMatrix and dimensions.
		 * @param M Pointer to @ref BlockedMatrix of which to construct submatrix
		 * @param rowbeg Starting block row
		 * @param colbeg Starting block column
		 * @param m rows of blocks 
		 * @param n cols of blocks
		 */
		AlignedBlockedSubmatrix(
			constMatrixType& M,
			const size_t rowbeg,
			const size_t colbeg,
			const size_t m,
			const size_t n) :
			// Init list begins here.
			_rows(m),
			_cols(n),
			_block_rows(M.blockRowdim()),
			_block_cols(M.blockColdim()),
			_r0(rowbeg),
			_c0(colbeg),
			_Mat(M){}

		AlignedBlockedSubmatrix(
			matrixType& M,
			const size_t rowbeg,
			const size_t colbeg,
			const size_t m,
			const size_t n) :
			// Init list begins here.
			_rows(m),
			_cols(n),
			_block_rows(M.blockRowdim()),
			_block_cols(M.blockColdim()),
			_r0(rowbeg),
			_c0(colbeg),
			_Mat(M){}

		/** Constructor from an existing @ref ALignedBlockedSubmatrix.
		 * @param rhs Pointer to @ref AlignedBlockedSubmatrix of which to construct submatrix
		 */
		AlignedBlockedSubmatrix(const AlignedBlockedSubmatrix& rhs) :
			// Init list begins here.
			_rows(rhs._rows),
			_cols(rhs._cols),
			_block_rows(rhs._block_rows),
			_block_cols(rhs._block_cols),
			_r0(rhs._r0),
			_c0(rhs._c0),
			_Mat(rhs._Mat){}
	
		//////////////////
		//  DIMENSIONS  //
		//////////////////

		/** Get the overall number of rows in the submatrix.
		 * @returns Overall number of rows in submatrix
		 */
		size_t rowdim() const{
			return _rows * _block_rows;
		}

		/** Get the overall number of columns in the submatrix.
		 * @returns Overall number of columns in submatrix
		 */
		size_t coldim() const{
			return _cols * _block_cols;
		}

		/** Get the number of block rows in the submatrix.
		 * @returns Number of block rows in submatrix
		 */
		size_t blockRowdim() const{
			return _rows;
		}

		/** Get the number of block columns in the submatrix.
		 * @returns Number of block columns in submatrix
		 */
		size_t blockColdim() const{
			return _cols;
		}

		/** Get the number of rows per block in the submatrix.
		 * @returns Number of rows per block in submatrix
		 */
		size_t rowsPerBlock() const{
			return _block_rows;
		}

		/** Get the number of columns per block in the submatrix.
		 * @returns Number of columns per block in submatrix
		 */
		size_t colsPerBlock() const{
			return _block_cols;
		}

		//////////////////
		//    BLOCKS    //
		//////////////////

		/** Set the block at the (i, j) position to a_ij.
		 * @param i Block Row number, 0...blockRowdim() - 1
		 * @param j Block Column number 0...blockColdim() - 1
		 * @param a_ij Block to set
		 */
		void setBlock(size_t i, size_t j, const Block_t& a_ij){
			_Mat.setBlock(_r0 + i, _c0 + j, a_ij);
		}

		/** Get a writeable reference to the block in the (i, j) position.
		 * @param i Block Row index of entry
		 * @param j Block Column index of entry
		 * @returns Reference to block
		 */
		Block_t& refBlock(size_t i, size_t j){
			return _Mat.refBlock(_r0 + i, _c0 + j);
		}

		/** Get a read-only reference to the block in the (i, j) position.
		 * @param i Block Row index
		 * @param j Block Column index
		 * @returns Const reference to block
		 */
		const Block_t& getBlock(size_t i, size_t j){
			return refBlock(i,j);
		}

		/** Copy the (i, j) block into x, and return a reference to x.
		 * This form is more in the Linbox style and is provided for interface
		 * compatibility with other parts of the library
		 * @param x Block in which to store result
		 * @param i Block Row index
		 * @param j Block Column index
		 * @returns Reference to x
		 */
		Block_t& getBlock(Block_t& x, size_t i, size_t j){
			x = getBlock(i,j);
			return x;
		}

		//////////////////
		//   ELEMENTS   //
		//////////////////

		/** Set the entry at the (i, j) position to a_ij.
		 * @param i Row number, 0...rowdim () - 1
		 * @param j Column number 0...coldim () - 1
		 * @param a_ij Element to set
		 */
		void setEntry(size_t i, size_t j, const Element& a_ij){
			size_t block_i = i / _block_rows;
			size_t block_j = j / _block_rows;
			i = i % _block_rows;
			j = j % _block_rows;
			(*refBlock(_r0 + block_i, _c0 + block_j)).setEntry(i,j,a_ij);
		}

		/** Get a writeable reference to the entry in the (i, j) position.
		 * @param i Row index of entry
		 * @param j Column index of entry
		 * @returns Reference to matrix entry
		 */
		Element& refEntry(size_t i, size_t j){
			size_t block_i = i / _block_rows;
			size_t block_j = j / _block_rows;
			i = i % _block_rows;
			j = j % _block_rows;
			return (*refBlock(_r0 + block_i, _c0 + block_j)).refEntry(i,j);
		}

		/** Get a read-only reference to the entry in the (i, j) position.
		 * @param i Row index
		 * @param j Column index
		 * @returns Const reference to matrix entry
		 */
		const Element& getEntry(size_t i, size_t j) const{
			return refEntry(i,j);
		}

		/** Copy the (i, j) entry into x, and return a reference to x.
		 * This form is more in the Linbox style and is provided for interface
		 * compatibility with other parts of the library
		 * @param x Element in which to store result
		 * @param i Row index
		 * @param j Column index
		 * @returns Reference to x
		 */
		Element& getEntry(Element& x, size_t i, size_t j) const{
			x = getEntry(i,j);
			return x;
		}

		//////////////////
		//   UTILITIES  //
		//////////////////

		/** Copy all elements of the input matrix into the blocked matrix.
		 * This assumes that the dimensions of the input are less than or
		 * the same as the dimensions of the blocked matrix.
		 * @param m Input matrix
		 * @returns Reference to this blocked matrix
		 */
		template<class _M>
		Self_t& copyFromMatrix(const _M& m){
			size_t rows = m.rowdim();
			size_t cols = m.coldim();

			for(size_t i = 0; i < rows; i++){
				for(size_t j = 0; j < cols; j++){
					setEntry(i,j,m.getEntry(i,j));
				}
			}

			return *this;
		}

		/** Copy all elements of the blocked matrix into the provided matrix.
		 * This assumes that the dimensions of the blocked matrix are
		 * less than or the same as the dimensions of the provided matrix.
		 * @param m A matrix
		 * @returns Reference to this blocked matrix
		 */
		template<class _M>
		Self_t& copyToMatrix(_M& m){
			size_t rows = rowdim();
			size_t cols = coldim();

			for(size_t i = 0; i < rows; i++){
				for(size_t j = 0; j < cols; j++){
					m.setEntry(i,j,getEntry(i,j));
				}
			}

			return *this;
		}


	}; // end of class AlignedBlockedSubmatrix
} // end of namespace LinBox

#endif // __LINBOX_matrix_blockedmatrix_blocked_matrix_H

// Local Variables:
// mode: C++
// tab-width: 8
// indent-tabs-mode: nil
// c-basic-offset: 8
// End:
// vim:sts=8:sw=8:ts=8:noet:sr:cino=>s,f0,{0,g0,(0,\:0,t0,+0,=s