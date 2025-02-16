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

/// @file GPSWeekZcount.cpp

#include "GPSWeekZcount.hpp"
#include "TimeConstants.hpp"
#include "TimeConverters.hpp"
#include "GPSWeekSecond.hpp"

namespace gnsstk
{
      // Not really keen on having the day of week names in
      // GPSWeekZcount but this is where the %w translation takes
      // place.
   static const char *weekdayAbbr[] =
   {
      "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
   };
   static const char *weekday[] =
   {
      "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday",
      "Saturday"
   };

   GPSWeekZcount& GPSWeekZcount::operator=( const GPSWeekZcount& right )
   {
      GPSWeek::operator=(right);
      zcount = right.zcount;
      return *this;
   }

   CommonTime GPSWeekZcount::convertToCommonTime() const
   {
      try
      {
         int dow = zcount / ZCOUNT_PER_DAY;
         int jday = MJD_JDAY + GPS_EPOCH_MJD + ( 7 * week ) + dow;
         double sod = static_cast<double>( zcount % ZCOUNT_PER_DAY ) * 1.5;
         CommonTime ct;
         return ct.set( jday,
                        static_cast<long>( sod ),
                        sod - static_cast<long>( sod ),
                        timeSystem );
      }
      catch (InvalidParameter& ip)
      {
         InvalidRequest ir(ip);
         GNSSTK_THROW(ir);
      }
   }

   void GPSWeekZcount::convertFromCommonTime( const CommonTime& ct )
   {
         /// This is the earliest CommonTime representable by GPSWeekZcount.
     static const CommonTime MIN_CT = GPSWeekZcount(0,0,TimeSystem::Any);

      if (ct < MIN_CT)
      {
         InvalidRequest ir("Unable to convert CommonTime to GPSWeekZcount.");
         GNSSTK_THROW(ir);
      }

      long day, sod;
      double fsod;
      ct.get( day, sod, fsod, timeSystem );

         // find the number of days since the beginning of the GPS Epoch
      day -= MJD_JDAY + GPS_EPOCH_MJD;
         // find out how many weeks that is
      week = static_cast<int>( day / 7 );
         // find out what the day of week is
      day %= 7;

      zcount = static_cast<long>( day * ZCOUNT_PER_DAY )
         + static_cast<long>( static_cast<double>( sod + fsod ) / 1.5 );
   }

   std::string GPSWeekZcount::printf( const std::string& fmt ) const
   {
      try
      {
         using gnsstk::StringUtils::formattedPrint;

         std::string rv = GPSWeek::printf( fmt );

         rv = formattedPrint( rv, getFormatPrefixInt() + "a",
                              "as", weekdayAbbr[getDayOfWeek()] );
         rv = formattedPrint( rv, getFormatPrefixInt() + "A",
                              "As", weekday[getDayOfWeek()] );
         rv = formattedPrint( rv, getFormatPrefixInt() + "w",
                              "wu", getDayOfWeek() );
         rv = formattedPrint( rv, getFormatPrefixInt() + "z",
                              "zu", zcount );
         rv = formattedPrint( rv, getFormatPrefixInt() + "Z",
                              "Zu", zcount );
         rv = formattedPrint( rv, getFormatPrefixInt() + "c",
                              "cu", getZcount29() );
         rv = formattedPrint( rv, getFormatPrefixInt() + "C",
                              "Cu", getZcount32() );
         rv = formattedPrint( rv, getFormatPrefixInt() + "P",
                              "Ps", StringUtils::asString(timeSystem).c_str() );
         return rv;
      }
      catch( gnsstk::StringUtils::StringException& exc )
      {
         GNSSTK_RETHROW( exc );
      }
   }

   std::string GPSWeekZcount::printError( const std::string& fmt ) const
   {
      try
      {
         using gnsstk::StringUtils::formattedPrint;

         std::string rv = GPSWeek::printError( fmt );

         rv = formattedPrint( rv, getFormatPrefixInt() + "w",
                              "ws", getError().c_str() );
         rv = formattedPrint( rv, getFormatPrefixInt() + "z",
                              "zs", getError().c_str() );
         rv = formattedPrint( rv, getFormatPrefixInt() + "Z",
                              "Zs", getError().c_str() );
         rv = formattedPrint( rv, getFormatPrefixInt() + "c",
                              "cs", getError().c_str() );
         rv = formattedPrint( rv, getFormatPrefixInt() + "C",
                              "Cs", getError().c_str() );
         rv = formattedPrint( rv, getFormatPrefixInt() + "P",
                              "Ps", getError().c_str() );
         return rv;
      }
      catch( gnsstk::StringUtils::StringException& exc )
      {
         GNSSTK_RETHROW( exc );
      }
   }

   bool GPSWeekZcount::setFromInfo( const IdToValue& info )
   {
      using namespace gnsstk::StringUtils;

      GPSWeek::setFromInfo(info);

      for( IdToValue::const_iterator i = info.begin(); i != info.end(); i++ )
      {
            // based on the character, we know what to do...
         switch( i->first )
         {
            case 'F':
               week = asInt( i->second );
               break;
            case 'w':
               zcount = asInt( i->second) * ZCOUNT_PER_DAY;
               break;

            case 'z':
            case 'Z':
               zcount = asInt( i->second );
               break;

            case 'c':
               setZcount29( asInt( i->second ) );
               break;

            case 'C':
               setZcount32( asInt( i->second ) );
               break;

            case 'P':
               timeSystem = gnsstk::StringUtils::asTimeSystem(i->second);
               break;

            default:
                  // do nothing
               break;
         };
      }

      return true;
   }

  //inline bool GPSWeekZcount::isValid() const
  //{
  //    return ( GPSWeek::isValid() &&
  //             zcount < ZCOUNT_PER_WEEK );
  //}

  //void GPSWeekZcount::reset()
  //{
  //    GPSWeek::reset();
  //    zcount = 0;
  //}

   GPSWeekZcount& GPSWeekZcount::setZcount29(unsigned int z)
   {
      setWeek10( (z >> 19) & bits10 );
      zcount = z & bits19;
      return *this;
   }

   GPSWeekZcount& GPSWeekZcount::setZcount32(unsigned int z)
   {
      week = z >> 19;
      zcount = z & bits19;
      return *this;
   }


   GPSWeekZcount& GPSWeekZcount::addWeeks(short inWeeks)
   {
      week += inWeeks;
      if (week < 0)
      {
         gnsstk::InvalidRequest exc("addWeeks results in negative week");
         GNSSTK_THROW(exc);
      }
      return *this;
   }


   GPSWeekZcount& GPSWeekZcount::addZcounts(long inZcounts)
   {
      if (inZcounts == 0)
         return *this;

      short originalWeek(week);
      long originalZcount(zcount);

      try
      {
            // First, do week modifications.
         addWeeks(inZcounts / ZCOUNT_PER_WEEK);

            // Now, take care of Z-counts.
         long tmp = zcount + (inZcounts % ZCOUNT_PER_WEEK);

         if (tmp < 0)
         {
            addWeeks(-1);
            tmp += ZCOUNT_PER_WEEK;
         }
         else if (tmp >= ZCOUNT_PER_WEEK)
         {
            addWeeks(1);
            tmp -= ZCOUNT_PER_WEEK;
         }

         zcount = tmp;
         return *this;
      }
      catch (gnsstk::InvalidRequest& ir)
      {
         week = originalWeek;
         zcount = originalZcount;
         ir.addText("Did not add " + StringUtils::asString(inZcounts) +
                    " Z-counts.");
         GNSSTK_RETHROW(ir);
      }
      catch (gnsstk::InvalidParameter& ip)
      {
         week = originalWeek;
         zcount = originalZcount;
         gnsstk::InvalidRequest ir(ip);
         ir.addText("Did not add " + StringUtils::asString(inZcounts) +
                    " Z-counts.");
         GNSSTK_THROW(ir);
      }
   }


   GPSWeekZcount GPSWeekZcount::operator++(int)
   {
      GPSWeekZcount temp = *this;
      ++(*this);
      return temp;
   }


   GPSWeekZcount& GPSWeekZcount::operator++()
   {
      return addZcounts(1);
   }


   GPSWeekZcount GPSWeekZcount::operator--(int)
   {
      GPSWeekZcount temp = *this;
      --(*this);
      return temp;
   }


   GPSWeekZcount& GPSWeekZcount::operator--()
   {
      return addZcounts(-1);
   }


   GPSWeekZcount GPSWeekZcount::operator+(long inZcounts) const
   {
      return GPSWeekZcount(*this).addZcounts(inZcounts);
   }


   GPSWeekZcount GPSWeekZcount::operator-(long inZcounts) const
   {
      return operator+(-inZcounts);
   }


   long GPSWeekZcount::operator-(const GPSWeekZcount& right) const
      noexcept
   {
      return (((long(week) - long(right.week)) * ZCOUNT_PER_WEEK) +
              (long(zcount) - long(right.zcount)));
   }


   GPSWeekZcount& GPSWeekZcount::operator+=(long inZcounts)
   {
      return addZcounts(inZcounts);
   }


   GPSWeekZcount& GPSWeekZcount::operator-=(long inZcounts)
   {
      return addZcounts(-inZcounts);
   }


   bool GPSWeekZcount::inSameTimeBlock(const GPSWeekZcount& other,
                                       unsigned long inZcountBlock,
                                       unsigned long inZcountOffset)
      noexcept
   {
      if (inZcountBlock < ZCOUNT_PER_WEEK)
      {
            // Make sure that we're in the same week, and then checkto
            // see if we're in the same time block.
         if ((week == other.week) &&
             (((zcount - inZcountOffset) / inZcountBlock) ==
              ((other.zcount - inZcountOffset) / inZcountBlock)))
         {
            return true;
         }
      }
      else
      {
            // Comare using the total number of Z-counts
         if (long((getTotalZcounts() - inZcountOffset) / inZcountBlock) ==
             long((other.getTotalZcounts() - inZcountOffset) / inZcountBlock))
         {
            return true;
         }
      }
      return false;
   }

} // namespace
