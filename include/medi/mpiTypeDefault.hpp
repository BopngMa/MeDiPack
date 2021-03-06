/*
 * MeDiPack, a Message Differentiation Package
 *
 * Copyright (C) 2017 Chair for Scientific Computing (SciComp), TU Kaiserslautern
 * Homepage: http://www.scicomp.uni-kl.de
 * Contact:  Prof. Nicolas R. Gauger (codi@scicomp.uni-kl.de)
 *
 * Lead developers: Max Sagebaum (SciComp, TU Kaiserslautern)
 *
 * This file is part of MeDiPack (http://www.scicomp.uni-kl.de/software/codi).
 *
 * MeDiPack is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * MeDiPack is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU
 * General Public License along with MeDiPack.
 * If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Max Sagebaum (SciComp, TU Kaiserslautern)
 */

#pragma once

#include "macros.h"
#include "mpiTypeInterface.hpp"
#include "mpiOp.hpp"

namespace medi {

  template<typename ADTool>
  class MpiTypeDefault final
      : public MpiTypeBase<
          MpiTypeDefault<ADTool>,
          typename ADTool::Type,
          typename ADTool::ModifiedType,
          ADTool>
  {

    public:

      typedef typename ADTool::Type Type;
      typedef typename ADTool::ModifiedType ModifiedType;
      typedef typename ADTool::PassiveType PassiveType;
      typedef typename ADTool::AdjointType AdjointType;
      typedef typename ADTool::IndexType IndexType;

      typedef ADTool Tool;

      Tool adTool;

      MpiTypeDefault() :
        MpiTypeBase<MpiTypeDefault<ADTool>, Type, ModifiedType, Tool>(Tool::MpiType, Tool::ModifiedMpiType),
        adTool(Tool::AdjointMpiType) {}

      Tool& getADTool() {
        return adTool;
      }

      int computeActiveElements(const int count) const {
        return count;
      }

      bool isModifiedBufferRequired() const {
        return Tool::IS_RequiresModifiedBuffer;
      }

      inline void copyIntoModifiedBuffer(const Type* buf, size_t bufOffset, ModifiedType* bufMod, size_t bufModOffset, int elements) const {
        if(ADTool::IS_RequiresModifiedBuffer) {
          for(int i = 0; i < elements; ++i) {
            ADTool::setIntoModifyBuffer(bufMod[bufModOffset + i], buf[bufOffset + i]);
          }
        }
      }

      inline void copyFromModifiedBuffer(Type* buf, size_t bufOffset, const ModifiedType* bufMod, size_t bufModOffset, int elements) const {
        if(ADTool::IS_RequiresModifiedBuffer) {
          for(int i = 0; i < elements; ++i) {
            ADTool::getFromModifyBuffer(bufMod[bufModOffset + i], buf[bufOffset + i]);
          }
        }
      }

      inline void getIndices(const Type* buf, size_t bufOffset, IndexType* indices, size_t bufModOffset, int elements) const {
        int indexOffset = computeActiveElements((int)bufModOffset);

        for(int i = 0; i < elements; ++i) {
          indices[indexOffset + i] = ADTool::getIndex(buf[bufOffset + i]);
        }
      }

      inline void registerValue(Type* buf, size_t bufOffset, IndexType* indices, PassiveType* oldPrimals, size_t bufModOffset, int elements) const {
        int indexOffset = computeActiveElements((int)bufModOffset);

        for(int i = 0; i < elements; ++i) {
          indices[indexOffset + i] = ADTool::registerValue(buf[bufOffset + i], oldPrimals[indexOffset + i]);
        }
      }

      inline void clearIndices(Type* buf, size_t bufOffset, int elements) const {
        for(int i = 0; i < elements; ++i) {
          ADTool::clearIndex(buf[bufOffset + i]);
        }
      }

      inline void getValues(const Type* buf, size_t bufOffset, PassiveType* primals, size_t bufModOffset, int elements) const {
        int primalOffset = computeActiveElements((int)bufModOffset);

        for(int pos = 0; pos < elements; ++pos) {
          primals[primalOffset + pos] = ADTool::getValue(buf[bufOffset + pos]);
        }
      }

      inline void performReduce(Type* buf, Type* target, int count, AMPI_Op op, int ranks) const {
        for(int j = 1; j < ranks; ++j) {
          MPI_Reduce_local(&buf[j * count], buf, count, this->getMpiType(), op.primalFunction);
        }

        copy(buf, 0, target, 0, count);
      }

      void copy(Type* from, size_t fromOffset, Type* to, size_t toOffset, int count) const {
        for(int i = 0; i < count; ++i) {
          to[toOffset + i] = from[fromOffset + i];
        }

      }

      inline void createTypeBuffer(Type* &buf, size_t size) const {
        buf = new Type[size];
      }

      inline void createModifiedTypeBuffer(ModifiedType* &buf, size_t size) const {
        buf = new ModifiedType[size];
      }

      inline void deleteTypeBuffer(Type* &buf) const {
        if(NULL != buf) {
          delete [] buf;
          buf = NULL;
        }
      }

      inline void deleteModifiedTypeBuffer(ModifiedType* &buf) const {
        if(NULL != buf) {
          delete [] buf;
          buf = NULL;
        }
      }
  };
}
