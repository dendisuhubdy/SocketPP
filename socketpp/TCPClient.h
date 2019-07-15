//
// Created by liushuai on 2019/7/13.
//

#pragma once

#include <cstring>
#include <thread>
#include <mutex>
#include <vector>
#include <queue>
#include <condition_variable>
#include <functional>

#include "SocketClient.h"
#include "Message.h"

namespace SocketPP {

class TCPClient : public SocketClient {
public:
    typedef std::function<bool(const Message &message)> MessageInterceptor;
    typedef std::function<void(const Message &message)> MessageHandle;
    typedef std::function<void(const TCPStream &stream)> StreamHandle;

    enum SendResult {
        NotConnected = -2,
        SendError = -1,
        Intercepted = 0,
    };

public:
    explicit TCPClient(const std::string& ip, int port = 6000);

    /**
     * send message immediately with thread safe
     * @param message
     * @return >0 : the length has send
     *          0 : if be intercept
     *         -1 : send error(may stream closed)
     *         -2 : not inited
     *         -3 : stream not found
     */
    ssize_t send(const Message &message);

    ssize_t send(const std::string &str);

    // post message to queue will be send automatic later
    void post(const Message &message);

    // flush all posted messages
    void flush();

    void disconnect();

    /**
     * @param interceptor return true: intercept it, false or not.
     */
    void setSendInterceptor(const MessageInterceptor &interceptor);

    void setSendHandle(const MessageHandle &handle);
    void setRecvHandle(const MessageHandle &handle);
    void setConnHandle(const StreamHandle &handle);
    void setDiscHandle(const StreamHandle &handle);

    void onReceive(const Message &message);

public:
    virtual void onStart(int efd) override;
    virtual void onConnected(int fd) override;
    virtual void onDisconnected(int fd) override;
    virtual void onReceive(int fd, const byte *buf, size_t len) override;

private:
    void startSendThread();

    void onSend(const Message &message);

private:
    bool _connected = false;

    TCPStream _streamPeer;

    std::queue<Message> _msgQueue;
    std::mutex _msgQueueMutex;
    std::condition_variable _msgQueueCondition;

    MessageInterceptor _sendInterceptor = nullptr;
    MessageHandle _sendHandle = nullptr;
    MessageHandle _recvHandle = nullptr;

    StreamHandle _connHandle = nullptr;
    StreamHandle _discHandle = nullptr;
};

}   // namespace SocketPP
