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

/**
 * @file TimeSystemCorr.hpp
 * Encapsulate time system corrections, defined by header of RINEX 3
 * navigation file, including RINEX 2, and used to convert CommonTime
 * between systems.
 */

#ifndef GNSSTK_TIMESYSTEMCORRECTION_INCLUDE
#define GNSSTK_TIMESYSTEMCORRECTION_INCLUDE

#include "GNSSconstants.hpp"
#include "CommonTime.hpp"
#include "GPSWeekSecond.hpp"
#include "IRNWeekSecond.hpp"
#include "CivilTime.hpp"

namespace gnsstk
{

      /** Time System Corrections as defined in the RINEX version 3
       * Navigation header. */
   class TimeSystemCorrection
   {
   public:
         /// Supported time system correction types, cf. RINEX version 3 spec.
      enum CorrType
      {
         Unknown=0,
         GPUT,    ///< GPS  to UTC using A0, A1
         GAUT,    ///< GAL  to UTC using A0, A1
         SBUT,    ///< SBAS to UTC using A0, A1, incl. provider and UTC ID
         GLUT,    ///< GLO  to UTC using A0 = -TauC , A1 = 0
         GPGA,    ///< GPS  to GAL using A0 = A0G   , A1 = A1G
         GAGP,    ///< GPS  to GAL using A0 = A0G   , A1 = A1G
         GLGP,    ///< GLO  to GPS using A0 = TauGPS, A1 = 0
         QZGP,    ///< QZS  to GPS using A0, A1
         QZUT,    ///< QZS  to UTC using A0, A1
         BDUT,    ///< BDT  to UTC using A0, A1
         BDGP,    ///< BDT  to GPS using A0, A1  !! not in RINEX
         IRUT,    ///< IRN  to UTC using A0, A1
         IRGP     ///< IRN  to GPS using A0, A1
      };

         /// Empty constructor
      TimeSystemCorrection();

         /// Constructor from string
      TimeSystemCorrection(std::string str);

         //// Set members to known (even if invalid) values
      void init();

      void fromString(const std::string& str);

         /** Convert a pair of TimeSystem enums to a CorrType enum.
          * @param[in] src The source time system for the conversion.
          * @param[in] tgt The target time system for the conversion.
          * @param[out] ct The source/target time system paired enum.
          * @return false if src/tgt aren't available as a CorrType pair. */
      static bool convertTimeSystemToCorrType(
         TimeSystem src, TimeSystem tgt, TimeSystemCorrection::CorrType& ct);

         /// Return readable string version of CorrType
      std::string asString() const;

         /// Return 4-char string version of CorrType
      std::string asString4() const;

         /// dump
      void dump(std::ostream& s) const;

         /** Equal operator.
          * @warning Only tests type, not the full set of fields */
      inline bool operator==(const TimeSystemCorrection& tc) const
      { return type == tc.type; }

         /** Less than operator - required for map.find()
          * @warning Only tests type, not the full set of fields */
      inline bool operator<(const TimeSystemCorrection& tc) const
      { return type < tc.type; }

         /** Return true if this object provides the correction
         * necessary to convert between the two given time
         * systems.
         * @param ts1,ts2  TimeSystems of interest
         * @return true if this object will convert ts1 <=> ts2
         * @throw Exception if either TimeSystem is Unknown, or if
         *   they are identical. */
      bool isConverterFor(const TimeSystem& ts1, const TimeSystem& ts2) const;

         /** Compute the conversion (in seconds) at the given time for
          * this object (TimeSystemCorrection). The caller must ensure
          * that the input time has the appropriate TimeSystem, it
          * will determine the sign of the correction; it is such that
          * it should ALWAYS be ADDED to the input time.
          * For example, suppose this object is a "GPUT" (GPS=>UTC)
          * correction. Then
          *    ct(GPS) + Correction(ct) will yield ct(UTC), and
          *    ct(UTC) + Correction(ct) will yield ct(GPS).
          *    [That is, Correction(ct) in the two cases differ in sign]
          * @param[in] ct the time at which to compute the
          *   correction; the TimeSystem of ct will determine the sign
          *   of the correction.
          * @return the correction (sec) to be added to ct to change
          *   its TimeSystem.
          * @throw Exception if the input TimeSystem matches neither
          *   system in this object. */
      double Correction(const CommonTime& ct) const;

         /** Set the time system of refTime to the appropriate value
          * based on type (CorrType). */
      void fixTimeSystem();

         //// Member data
         ///  NOTE: User is responsible for setting the following parameters
         ///  after instantiation of a TimeSystemCorrection object and prior
         ///  to calling Correction( )
         ///    refWeek - must be in GPS full weeks (regardless of what GNSS
         ///              is being considered.)
         ///    refSOW
         ///    A0      - A0utc or A0gps for most system.  For GLONASS
         ///              GLUT corection supply -1.0 * tau_sub_c.
         ///              This convention is selected in order to maintain
         ///              consistency with the RINEX documentation in Table A5.
         ///    A1
      CorrType type;
      TimeSystem frTS,toTS;
      double A0, A1;
      CommonTime refTime;        ///< reference time for polynominal
      std::string geoProvider;   ///< string 'EGNOS' 'WAAS' or 'MSAS'
      int geoUTCid;              ///< UTC Identifier [0 unknown, 1=UTC(NIST),
                                 ///<  2=UTC(USNO), 3=UTC(SU), 4=UTC(BIPM),
                                 ///<  5=UTC(Europe), 6=UTC(CRL)]

   }; // End of class 'TimeSystemCorrection'

};    // end namespace

#endif // GNSSTK_TIMESYSTEMCORRECTION_INCLUDE
