/*
 * NiceConnection.h
 */

#ifndef NICECONNECTION_H_
#define NICECONNECTION_H_

#include <string>
#include <vector>
#include <boost/thread.hpp>

#include "MediaDefinitions.h"
#include "SdpInfo.h"
#include "logger.h"

typedef struct _NiceAgent NiceAgent;
typedef struct _GMainLoop GMainLoop;
typedef struct _GMainContext GMainContext;

typedef unsigned int uint;

namespace erizo {
//forward declarations
class CandidateInfo;
class WebRtcConnection;

/**
 * States of ICE
 */
enum IceState {
	NICE_INITIAL, NICE_CANDIDATES_GATHERED, NICE_CANDIDATES_RECEIVED, NICE_READY, NICE_FINISHED, NICE_FAILED
};

class NiceConnectionListener {
public:
	virtual void onNiceData(unsigned int component_id, char* data, int len, NiceConnection* conn)=0;
	virtual void updateIceState(IceState state, NiceConnection *conn)=0;
};

/**
 * An ICE connection via libNice
 * Represents an ICE Connection in an new thread.
 *
 */
class NiceConnection {
	DECLARE_LOGGER();
public:

	/**
	 * Constructs a new NiceConnection.
	 * @param med The MediaType of the connection.
	 * @param transportName The name of the transport protocol. Was used when WebRTC used video_rtp instead of just rtp.
   * @param iceComponents Number of ice components pero connection. Default is 1 (rtcp-mux).
	 */
	NiceConnection(MediaType med, const std::string &transportName, unsigned int iceComponents=1,
		const std::string& stunServer = "", int stunPort = 3478, int minPort = 0, int maxPort = 65535);

	virtual ~NiceConnection();
	/**
	 * Starts Gathering candidates in a new thread.
	 */
	void start();
	/**
	 * Sets the remote ICE Candidates.
	 * @param candidates A vector containing the CandidateInfo.
	 * @return true if successfull.
	 */
	bool setRemoteCandidates(std::vector<CandidateInfo> &candidates);
	/**
	 * Sets the local ICE Candidates. Called by C Nice functions.
	 * @param candidates A vector containing the CandidateInfo.
	 * @return true if successfull.
	 */
	void gatheringDone(uint stream_id);
	/**
	 * Sets the associated Listener.
	 * @param connection Pointer to the NiceConnectionListener.
	 */
	void setNiceListener(NiceConnectionListener *listener);

	/**
	 * Gets the associated Listener.
	 * @param connection Pointer to the NiceConnectionListener.
	 */
	NiceConnectionListener* getNiceListener();
	/**
	 * Sends data via the ICE Connection.
	 * @param buf Pointer to the data buffer.
	 * @param len Length of the Buffer.
	 * @return Bytes sent.
	 */
	int sendData(unsigned int compId, const void* buf, int len);

	/**
	 * The MediaType of the connection
	 */
	MediaType mediaType;
	/**
	 * The transport name
	 */
  boost::scoped_ptr<std::string> transportName;
	/**
	 * The state of the ice Connection
	 */
	IceState iceState;
	/**
	 * The Obtained local candidates.
	 */
  boost::shared_ptr<std::vector<CandidateInfo> > localCandidates;

	void updateIceState(IceState state);
	void updateComponentState(unsigned int compId, IceState state);


private:
	void init();
	NiceAgent* agent_;
	NiceConnectionListener* listener_;
	GMainLoop* loop_;
	GMainContext* context_;
	boost::thread m_Thread_;
	boost::mutex writeMutex_;
  	unsigned int iceComponents_;
  	std::map <unsigned int, IceState> comp_state_list;
	std::string stunServer_;
	int stunPort_, minPort_, maxPort_;
};

} /* namespace erizo */
#endif /* NICECONNECTION_H_ */
