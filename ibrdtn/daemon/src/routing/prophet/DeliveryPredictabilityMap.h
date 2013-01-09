/*
 * DeliveryPredictabilityMap.h
 *
 *  Created on: 08.01.2013
 *      Author: morgenro
 */

#ifndef DELIVERYPREDICTABILITYMAP_H_
#define DELIVERYPREDICTABILITYMAP_H_

#include "routing/NeighborDataset.h"
#include "routing/NodeHandshake.h"
#include <ibrdtn/data/EID.h>
#include <ibrcommon/thread/Mutex.h>
#include <map>

namespace dtn
{
	namespace routing
	{
		/*!
		 * \brief This class keeps track of the predictablities to see a specific EID
		 *
		 * This class can be used as a map from EID to float.
		 * Also, it can be serialized as a NodeHandshakeItem to be exchanged with neighbors.
		 */
		class DeliveryPredictabilityMap : public NeighborDataset, public NodeHandshakeItem, public ibrcommon::Mutex {
		public:
			static const size_t identifier;

			DeliveryPredictabilityMap();
			DeliveryPredictabilityMap(const size_t &time_unit, const float &beta, const float &gamma);
			virtual ~DeliveryPredictabilityMap();

			virtual size_t getIdentifier() const; ///< \see NodeHandshakeItem::getIdentifier
			virtual size_t getLength() const; ///< \see NodeHandshakeItem::getLength
			virtual std::ostream& serialize(std::ostream& stream) const; ///< \see NodeHandshakeItem::serialize
			virtual std::istream& deserialize(std::istream& stream); ///< \see NodeHandshakeItem::deserialize

			class ValueNotFoundException : public ibrcommon::Exception
			{
			public:
				ValueNotFoundException()
				: ibrcommon::Exception("The requested value is not available.") { };

				virtual ~ValueNotFoundException() throw () { };
			};

			float get(const dtn::data::EID &neighbor) const throw (ValueNotFoundException);
			void set(const dtn::data::EID &neighbor, float value);
			void clear();

			/*!
			 * Updates the DeliveryPredictabilityMap with one recieved by a neighbor.
			 * \param dpm the DeliveryPredictabilityMap recieved from the neighbor
			 * \warning The _deliveryPredictabilityMap has to be locked before calling this function
			 */
			void update(const dtn::data::EID &origin, const DeliveryPredictabilityMap &dpm, const float &p_encounter_first);

			/*!
			 * Age all entries in the DeliveryPredictabilityMap.
			 * \warning The _deliveryPredictabilityMap has to be locked before calling this function
			 */
			void age(const float &p_first_threshold);

			/**
			 * Print out the content as readable text.
			 */
			void toString(std::ostream &stream) const;

			friend std::ostream& operator<<(std::ostream& stream, const DeliveryPredictabilityMap& map);

		private:
			typedef std::map<dtn::data::EID, float> predictmap;
			predictmap _predictmap;

			float _beta; ///< Weight of the transitive property of prophet.
			float _gamma; ///< Determines how quickly predictabilities age.

			size_t _lastAgingTime; ///< Timestamp when the map has been aged the last time.
			size_t _time_unit; ///< time unit to be used in the network

		};
	} /* namespace routing */
} /* namespace dtn */
#endif /* DELIVERYPREDICTABILITYMAP_H_ */
