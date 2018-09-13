#include <fastcdr/Cdr.h>
#include <fastcdr/exceptions/BadParamException.h>
#include <gtest/gtest.h>

#include "cybertron/common/global_data.h"
#include "cybertron/common/log.h"
#include "cybertron/transport/qos/qos_profile_conf.h"
#include "cybertron/transport/rtps/attributes_filler.h"
#include "cybertron/transport/rtps/participant.h"
#include "cybertron/transport/rtps/underlay_message.h"
#include "cybertron/transport/rtps/underlay_message_type.h"

namespace apollo {
namespace cybertron {
namespace transport {

TEST(AttributesFillerTest, fill_in_pub_attr_test) {
  QosProfile qos;
  AttributesFiller filler;
  eprosima::fastrtps::PublisherAttributes attrs;
  qos.set_history(QosHistoryPolicy::HISTORY_KEEP_LAST);
  qos.set_durability(QosDurabilityPolicy::DURABILITY_TRANSIENT_LOCAL);
  qos.set_reliability(QosReliabilityPolicy::RELIABILITY_BEST_EFFORT);
  qos.set_mps(32);
  filler.FillInPubAttr("channel", qos, &attrs);
  EXPECT_EQ(eprosima::fastrtps::KEEP_LAST_HISTORY_QOS,
            attrs.topic.historyQos.kind);
  EXPECT_EQ(eprosima::fastrtps::TRANSIENT_LOCAL_DURABILITY_QOS,
            attrs.qos.m_durability.kind);
  EXPECT_EQ(eprosima::fastrtps::BEST_EFFORT_RELIABILITY_QOS,
            attrs.qos.m_reliability.kind);
  AINFO << "heartbeat period: " << attrs.times.heartbeatPeriod.seconds << ", "
        << attrs.times.heartbeatPeriod.fraction;
  qos.set_depth(1024);
  attrs.topic.historyQos.depth = 512;
  filler.FillInPubAttr("channel", qos, &attrs);
  AINFO << qos.depth() << ", "
        << QosProfileConf::QOS_HISTORY_DEPTH_SYSTEM_DEFAULT << ", "
        << attrs.topic.historyQos.depth;
  EXPECT_EQ(qos.depth(), attrs.topic.historyQos.depth);

  qos.set_history(QosHistoryPolicy::HISTORY_KEEP_ALL);
  qos.set_durability(QosDurabilityPolicy::DURABILITY_VOLATILE);
  qos.set_reliability(QosReliabilityPolicy::RELIABILITY_RELIABLE);
  qos.set_mps(65);
  filler.FillInPubAttr("channel", qos, &attrs);
  EXPECT_EQ(eprosima::fastrtps::KEEP_ALL_HISTORY_QOS,
            attrs.topic.historyQos.kind);
  EXPECT_EQ(eprosima::fastrtps::VOLATILE_DURABILITY_QOS,
            attrs.qos.m_durability.kind);
  EXPECT_EQ(eprosima::fastrtps::RELIABLE_RELIABILITY_QOS,
            attrs.qos.m_reliability.kind);
  AINFO << "heartbeat period: " << attrs.times.heartbeatPeriod.seconds << ", "
        << attrs.times.heartbeatPeriod.fraction;

  qos.set_history(QosHistoryPolicy::HISTORY_SYSTEM_DEFAULT);
  qos.set_durability(QosDurabilityPolicy::DURABILITY_SYSTEM_DEFAULT);
  qos.set_reliability(QosReliabilityPolicy::RELIABILITY_SYSTEM_DEFAULT);
  qos.set_mps(1025);
  filler.FillInPubAttr("channel", qos, &attrs);
  EXPECT_EQ(eprosima::fastrtps::KEEP_ALL_HISTORY_QOS,
            attrs.topic.historyQos.kind);
  EXPECT_EQ(eprosima::fastrtps::VOLATILE_DURABILITY_QOS,
            attrs.qos.m_durability.kind);
  EXPECT_EQ(eprosima::fastrtps::RELIABLE_RELIABILITY_QOS,
            attrs.qos.m_reliability.kind);
  AINFO << "heartbeat period: " << attrs.times.heartbeatPeriod.seconds << ", "
        << attrs.times.heartbeatPeriod.fraction;
  qos.set_mps(0);
  filler.FillInPubAttr("channel", qos, &attrs);
  AINFO << "heartbeat period: " << attrs.times.heartbeatPeriod.seconds << ", "
        << attrs.times.heartbeatPeriod.fraction;
}

TEST(AttributesFillerTest, fill_in_sub_attr_test) {
  QosProfile qos;
  AttributesFiller filler;
  eprosima::fastrtps::SubscriberAttributes attrs;
  qos.set_history(QosHistoryPolicy::HISTORY_KEEP_LAST);
  qos.set_durability(QosDurabilityPolicy::DURABILITY_TRANSIENT_LOCAL);
  qos.set_reliability(QosReliabilityPolicy::RELIABILITY_BEST_EFFORT);
  qos.set_mps(32);
  filler.FillInSubAttr("channel", qos, &attrs);
  EXPECT_EQ(eprosima::fastrtps::KEEP_LAST_HISTORY_QOS,
            attrs.topic.historyQos.kind);
  EXPECT_EQ(eprosima::fastrtps::TRANSIENT_LOCAL_DURABILITY_QOS,
            attrs.qos.m_durability.kind);
  EXPECT_EQ(eprosima::fastrtps::BEST_EFFORT_RELIABILITY_QOS,
            attrs.qos.m_reliability.kind);
  qos.set_depth(1024);
  attrs.topic.historyQos.depth = 512;
  filler.FillInSubAttr("channel", qos, &attrs);
  EXPECT_EQ(qos.depth(), attrs.topic.historyQos.depth);

  qos.set_history(QosHistoryPolicy::HISTORY_KEEP_ALL);
  qos.set_durability(QosDurabilityPolicy::DURABILITY_VOLATILE);
  qos.set_reliability(QosReliabilityPolicy::RELIABILITY_RELIABLE);
  qos.set_mps(65);
  filler.FillInSubAttr("channel", qos, &attrs);
  EXPECT_EQ(eprosima::fastrtps::KEEP_ALL_HISTORY_QOS,
            attrs.topic.historyQos.kind);
  EXPECT_EQ(eprosima::fastrtps::VOLATILE_DURABILITY_QOS,
            attrs.qos.m_durability.kind);
  EXPECT_EQ(eprosima::fastrtps::RELIABLE_RELIABILITY_QOS,
            attrs.qos.m_reliability.kind);

  qos.set_history(QosHistoryPolicy::HISTORY_SYSTEM_DEFAULT);
  qos.set_durability(QosDurabilityPolicy::DURABILITY_SYSTEM_DEFAULT);
  qos.set_reliability(QosReliabilityPolicy::RELIABILITY_SYSTEM_DEFAULT);
  qos.set_mps(1025);
  filler.FillInSubAttr("channel", qos, &attrs);
  EXPECT_EQ(eprosima::fastrtps::KEEP_ALL_HISTORY_QOS,
            attrs.topic.historyQos.kind);
  EXPECT_EQ(eprosima::fastrtps::VOLATILE_DURABILITY_QOS,
            attrs.qos.m_durability.kind);
  EXPECT_EQ(eprosima::fastrtps::RELIABLE_RELIABILITY_QOS,
            attrs.qos.m_reliability.kind);
}

TEST(ParticipantTest, participant_test) {
  eprosima::fastrtps::ParticipantListener listener;
  eprosima::fastrtps::ParticipantListener listener1;
  char* domain_id = (char*)"CYBER_DOMAIN_ID=string";

  Participant participant1("channel", 8888, nullptr);
  unsetenv("CYBER_DOMAIN_ID");
  Participant participant2("channel", 8888, nullptr);
  putenv("CYBER_DOMAIN_ID=string");
  Participant participant3("channel", 8888, nullptr);
  putenv("CYBER_DOMAIN_ID=123");
  unsetenv("CYBER_IP");
  Participant participant4("channel", 8888, nullptr);
  putenv("CYBER_IP=127.0.0.256");
  Participant participant5("channel", 8888, nullptr);
  putenv("CYBER_IP=");
  Participant participant6("channel", 8888, nullptr);
  putenv("CYBER_IP=127.0.0.1");
  auto& conf = common::GlobalData::Instance()->config_;
  conf.mutable_transport_conf()->clear_participant_attr();
  Participant participant7("channel", 8888, nullptr);
  conf.clear_transport_conf();
  Participant participant8("channel", 8888, nullptr);
}

TEST(UnderlayMessageTest, underlay_message_test) {
  UnderlayMessage message;
  message.timestamp(1024);
  int32_t& t = message.timestamp();
  t = 256;
  EXPECT_EQ(256, message.timestamp());

  message.seq(1024);
  int32_t& seq = message.seq();
  seq = 256;
  EXPECT_EQ(256, message.seq());

  message.data("data");
  std::string& data = message.data();
  data = "data string";
  EXPECT_EQ(data, message.data());
  message.data(std::forward<std::string>("data forward"));
  EXPECT_EQ("data forward", message.data());

  message.datatype("datatype");
  std::string& datatype = message.datatype();
  datatype = "datatype string";
  EXPECT_EQ(datatype, message.datatype());
  message.datatype("datatype assign");
  EXPECT_EQ("datatype assign", message.datatype());
  message.datatype(std::forward<std::string>("datatype forward"));
  EXPECT_EQ("datatype forward", message.datatype());

  const UnderlayMessage const_message(message);
  int32_t t1 = const_message.timestamp();
  int32_t seq1 = const_message.seq();
  std::string data1 = const_message.data();
  std::string datatype1 = const_message.datatype();
  EXPECT_EQ(256, const_message.timestamp());
  EXPECT_EQ(256, const_message.seq());
  EXPECT_EQ("data forward", const_message.data());
  EXPECT_EQ("datatype forward", const_message.datatype());

  UnderlayMessage message2;
  message2 = message;
  EXPECT_EQ(256, message2.timestamp());
  EXPECT_EQ(256, message2.seq());
  EXPECT_EQ("data forward", message2.data());
  EXPECT_EQ("datatype forward", message2.datatype());

  UnderlayMessage message3;
  message3 = std::forward<UnderlayMessage>(message2);
  EXPECT_EQ(256, message3.timestamp());
  EXPECT_EQ(256, message3.seq());
  EXPECT_EQ("data forward", message3.data());
  EXPECT_EQ("datatype forward", message3.datatype());

  UnderlayMessage message4(message3);
  EXPECT_EQ(256, message4.timestamp());
  EXPECT_EQ(256, message4.seq());
  EXPECT_EQ("data forward", message4.data());
  EXPECT_EQ("datatype forward", message4.datatype());

  UnderlayMessage message5(std::forward<UnderlayMessage>(message4));
  EXPECT_EQ(256, message5.timestamp());
  EXPECT_EQ(256, message5.seq());
  EXPECT_EQ("data forward", message5.data());
  EXPECT_EQ("datatype forward", message5.datatype());

  EXPECT_EQ("", message4.data());
  EXPECT_EQ("", message4.datatype());
}

}  // namespace transport
}  // namespace cybertron
}  // namespace apollo

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
