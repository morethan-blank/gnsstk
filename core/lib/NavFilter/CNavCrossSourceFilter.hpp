//==============================================================================
//
//  This file is part of GNSSTk, the ARL:UT GNSS Toolkit.
//
//  The GNSSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
//  any later version.
//
//  The GNSSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GNSSTk; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin.
//  Copyright 2004-2022, The Board of Regents of The University of Texas System
//
//==============================================================================

//==============================================================================
//
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin, under contract to an agency or agencies
//  within the U.S. Department of Defense. The U.S. Government retains all
//  rights to use, duplicate, distribute, disclose, or release this software.
//
//  Pursuant to DoD Directive 523024
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public
//                            release, distribution is unlimited.
//
//==============================================================================

#ifndef CNAVCROSSOURCEFILTER_HPP
#define CNAVCROSSOURCEFILTER_HPP

#include "NavFilterMgr.hpp"
#include "NavFilter.hpp"
#include "CNavFilterData.hpp"

namespace gnsstk
{
      /// @ingroup NavFilter
      //@{

      /** Filter GPS subframes using a voting algorithm across a
       * single epoch.  This may be used for a single receiver where
       * multiple codes can be compared against each other, or across
       * multiple receivers (with or without multiple codes).
       *
       * @attention Processing depth = 2 epochs. */
   class CNavCrossSourceFilter : public NavFilter
   {
   public:
      CNavCrossSourceFilter();

         /** Add CNAV messages to the voting collection (groupedNav).
          * @pre NavFilterKey::timeStamp is set to either the the
          * time of transmission of the message/
          * @pre NavFilterKey::prn is set
          * @pre CNavFilterData::sf is set
          * @param[in,out] msgBitsIn A list of CNavFilterData* objects
          *   containing GPS CNAV messages.
          * @param[out] msgBitsOut The messages successfully passing
          *   the filter.  The contents of msgBitsOut will always be
          *   one epoch behind msgBitsIn (meaning data from previous,
          *   but not current calls to validate will be here). */
      virtual void validate(NavMsgList& msgBitsIn, NavMsgList& msgBitsOut);

         /** Flush the remaining contents of groupedNav.
          * @param[out] msgBitsOut Any remaining valid (by vote) nav
          *   messages are stored here on return. */
      virtual void finalize(NavMsgList& msgBitsOut);

         /// Internally stores 1 epoch's worth of subframe data.
      virtual unsigned processingDepth() const noexcept
      { return 1; }

         /// Return the filter name.
      virtual std::string filterName() const noexcept
      { return "CrossSource"; }

      virtual void setMinIdentical(const unsigned value)
      { minIdentical = value;}

      virtual unsigned short getMinIdentical () const
      { return minIdentical;}

      // Minimum # of identical messages needed
      unsigned short minIdentical;

         /** Debug method to unspool contents in a manner appropriate
          *  for inspection.   */
      virtual void dump(std::ostream& s) const;

   protected:
         /// Map from subframe data to source list
      typedef std::map<CNavFilterData*, NavMsgList, CNavMsgSort> MessageMap;
         /// Map from PRN to SubframeMap
      typedef std::map<uint32_t, MessageMap> NavMap;

         /// Nav subframes grouped by prn and unique nav bits
      NavMap groupedNav;
         /// Most recent time
      gnsstk::CommonTime currentTime;


         /** Filter by vote.
          * @note Bare minimum for producing output is 2 out of 2
          *   matching subframes.  If there are no matching subframes,
          *   or fewer than 2 subframes are present in groupedNav, no
          *   output will be produced.
          * @param[out] msgBitsOut Nav messages passing the voting
          *   algorithm are stored here. */
      void examineMessages(NavMsgList& msgBitsOut);
   };

      //@}

}

#endif // CNAVCROSSOURCEFILTER_HPP
