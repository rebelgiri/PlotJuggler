#ifndef DATASTREAM_ROS2_TOPIC_H
#define DATASTREAM_ROS2_TOPIC_H

#include <QtPlugin>
#include <QTimer>

#include "PlotJuggler/datastreamer_base.h"

#include "dialog_select_ros_topics.h"
#include "ros2_parsers/ros2_parser.h"
#include "ros2_parsers/generic_subscription.hpp"

class DataStreamROS2 : public DataStreamer
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.icarustechnology.PlotJuggler.ROS2DataStreamer" "../datastreamer.json")
    Q_INTERFACES(DataStreamer)

public:

    DataStreamROS2();

    virtual bool start(QStringList* selected_datasources) override;

    virtual void shutdown() override;

    virtual bool isRunning() const override;

    virtual ~DataStreamROS2() override;

    virtual const char* name() const override { return "ROS2 Topic Subscriber"; }

    virtual bool xmlSaveState(QDomDocument &doc, QDomElement &parent_element) const override;

    virtual bool xmlLoadState(const QDomElement &parent_element) override;

    virtual void addActionsToParentMenu(QMenu* menu) override;

    virtual std::vector<QString> appendData(PlotDataMapRef& destination) override
    {
        _destination_data = &destination;
        return DataStreamer::appendData(destination);
    }

  private:
    void messageCallback(const std::string& topic_name, TopicInfo &ti,
                         std::shared_ptr<rmw_serialized_message_t> msg);

private:

    PlotDataMapRef* _destination_data;

    std::shared_ptr<rclcpp::Context> _context;
    std::unique_ptr<rclcpp::executors::MultiThreadedExecutor> _executor;
    std::shared_ptr<rclcpp::Node> _node;

    Ros2Introspection::Parser _parser;

    bool _running;

    QTimer _spin_timer;

    std::unordered_map<std::string,TopicInfo> _topic_info;

    DialogSelectRosTopics::Configuration _config;

    rclcpp::Clock _clock;
    rcl_time_point_value_t _start_time;

    void saveDefaultSettings();

    void loadDefaultSettings();

    std::unordered_map<std::string, rosbag2_transport::GenericSubscription::Ptr> _subscriptions;

    void subscribeToTopic(const std::string& topic_name, const std::string& topic_type);
    void waitOneSecond();
};

#endif

