// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package k8s

import (
	"fmt"
	"strings"

	cspb "visionai/proto/cluster_selection_go_proto"
	acpb "google3/third_party/visionai/proto/lva/analyzer_config_go_proto"
	ccpb "google3/third_party/visionai/proto/lva/channel_config_go_proto"
	icpb "google3/third_party/visionai/proto/lva/io_config_go_proto"
	copb "google3/third_party/visionai/proto/util/net/grpc/connection_options_go_proto"

	durationpb "google3/google/protobuf/duration_go_proto"
	"google3/third_party/golang/protobuf/v2/encoding/prototext/prototext"
	"google3/third_party/visionai/golang/pkg/lva/program/compiler/asg/asg"
)

func genAisInputChannelConfig(seriesName, streamID string) (*ccpb.InputChannelConfig, error) {
	inputChannelConfig := ccpb.InputChannelConfig_builder{
		AisInputChannelConfig: ccpb.AisInputChannelConfig_builder{
			Series:       seriesName,
			SeriesServer: seriesServerEndpointTemplateString,
			ConnectionOptions: copb.ConnectionOptions_builder{
				ChannelOptions: copb.ConnectionOptions_ChannelOptions_builder{
					MaxReceiveMessageSize: -1,
					MaxSendMessageSize:    -1,
				}.Build(),
				ClientContextOptions: copb.ConnectionOptions_ClientContextOptions_builder{
					Timeout: &durationpb.Duration{
						Seconds: 900,
					},
					WaitForReady: true,
				}.Build(),
			}.Build(),
			ClusterSelection: cspb.ClusterSelection_builder{
				ClusterEndpoint: seriesServerEndpointTemplateString,
				ProjectId:       projectIDTemplateString,
				LocationId:      locationIDTemplateString,
				ClusterId:       clusterIDTemplateString,
			}.Build(),
			EventId:  eventIDTemplateString,
			StreamId: streamID,
			Lessee:   lesseeTemplateString,
		}.Build(),
	}.Build()
	return inputChannelConfig, nil
}

func genGrpcInputChannelConfig(streamName string) (*ccpb.InputChannelConfig, error) {
	tokens := strings.Split(streamName, ":")
	analyzerName, seriesName := tokens[0], tokens[1]
	upstreamOperator := fmt.Sprintf(
		"%v:%v",
		analyzerNameTemplate(analyzerName),
		internalGrpcOutputPort,
	)
	inputChannelConfig := ccpb.InputChannelConfig_builder{
		GrpcInputChannelConfig: ccpb.GrpcInputChannelConfig_builder{
			Series:           seriesName,
			UpstreamOperator: upstreamOperator,
		}.Build(),
	}.Build()
	return inputChannelConfig, nil
}

func genInputConfig(info *asg.AnalyzerInfo, ctx *Context) ([]*icpb.InputReceiverConfig, error) {
	// TODO: Treat StreamSource in a special way. Improve the IR to remove this.
	if info.Operator.Name == "StreamSource" {
		ctx.inputPlaceholderNames = append(ctx.inputPlaceholderNames, info.Name)
		seriesPlaceholderName := fmt.Sprintf(
			inputPlaceholderTemplateString,
			len(ctx.inputPlaceholderNames)-1,
		)
		streamPlaceholderID := fmt.Sprintf(inputStreamIDTemplateString, len(ctx.inputPlaceholderNames)-1)
		channelConfig, err := genAisInputChannelConfig(seriesPlaceholderName, streamPlaceholderID)
		if err != nil {
			return nil, err
		}
		inputReceiverConfigs := []*icpb.InputReceiverConfig{
			icpb.InputReceiverConfig_builder{
				ChannelConfig: channelConfig,
			}.Build(),
		}
		return inputReceiverConfigs, nil
	}

	inputReceiverConfigs := []*icpb.InputReceiverConfig{}
	for _, streamInfo := range info.InputStreams {
		channelConfig, err := genGrpcInputChannelConfig(streamInfo.Stream.Name)
		if err != nil {
			return nil, err
		}
		inputReceiverConfigs = append(inputReceiverConfigs,
			icpb.InputReceiverConfig_builder{
				ChannelConfig: channelConfig,
			}.Build())
	}
	return inputReceiverConfigs, nil
}

func genAisOutputChannelConfig(seriesName, streamID string) (*ccpb.OutputChannelConfig, error) {
	outputChannelConfig := ccpb.OutputChannelConfig_builder{
		AisOutputChannelConfig: ccpb.AisOutputChannelConfig_builder{
			Series:       seriesName,
			SeriesServer: seriesServerEndpointTemplateString,
			Retry:        10,
			ConnectionOptions: copb.ConnectionOptions_builder{
				ChannelOptions: copb.ConnectionOptions_ChannelOptions_builder{
					MaxReceiveMessageSize: -1,
					MaxSendMessageSize:    -1,
				}.Build(),
				ClientContextOptions: copb.ConnectionOptions_ClientContextOptions_builder{
					Timeout: &durationpb.Duration{
						Seconds: 900,
					},
					WaitForReady: true,
				}.Build(),
			}.Build(),
			ClusterSelection: cspb.ClusterSelection_builder{
				ClusterEndpoint: seriesServerEndpointTemplateString,
				ProjectId:       projectIDTemplateString,
				LocationId:      locationIDTemplateString,
				ClusterId:       clusterIDTemplateString,
			}.Build(),
			EventId:  eventIDTemplateString,
			StreamId: streamID,
			Lessee:   lesseeTemplateString,
		}.Build(),
	}.Build()
	return outputChannelConfig, nil
}

func genGrpcOutputChannelConfig(streamName string) (*ccpb.OutputChannelConfig, error) {
	tokens := strings.Split(streamName, ":")
	seriesName := tokens[1]
	outputChannelConfig := ccpb.OutputChannelConfig_builder{
		GrpcOutputChannelConfig: ccpb.GrpcOutputChannelConfig_builder{
			Series: seriesName,
		}.Build(),
	}.Build()
	return outputChannelConfig, nil
}

func genOutputConfig(info *asg.AnalyzerInfo, ctx *Context) ([]*icpb.OutputSenderConfig, error) {
	// TODO: Treat StreamSink in a special way. Improve the IR to remove this.
	if info.Operator.Name == "StreamSink" {
		ctx.outputPlaceholderNames = append(ctx.outputPlaceholderNames, info.Name)
		seriesPlaceholderName := fmt.Sprintf(
			outputPlaceholderTemplateString,
			len(ctx.outputPlaceholderNames)-1,
		)
		streamPlaceholderID := fmt.Sprintf(outputStreamIDTemplateString, len(ctx.outputPlaceholderNames)-1)
		channelConfig, err := genAisOutputChannelConfig(seriesPlaceholderName, streamPlaceholderID)
		if err != nil {
			return nil, err
		}
		outputSenderConfigs := []*icpb.OutputSenderConfig{
			icpb.OutputSenderConfig_builder{
				ChannelConfig: channelConfig,
			}.Build(),
		}
		return outputSenderConfigs, nil
	}

	outputSenderConfigs := []*icpb.OutputSenderConfig{}
	for _, streamInfo := range info.OutputStreams {
		channelConfig, err := genGrpcOutputChannelConfig(streamInfo.Stream.Name)
		if err != nil {
			return nil, err
		}
		outputSenderConfigs = append(outputSenderConfigs,
			icpb.OutputSenderConfig_builder{
				ChannelConfig: channelConfig,
			}.Build())
	}
	return outputSenderConfigs, nil
}

func genExecutorConfig(info *asg.AnalyzerInfo, ctx *Context) (*acpb.AnalyzerConfig_ExecutorConfig, error) {
	holdTime := info.Resources.LatencyBudgetMs
	switch {
	case holdTime < 0:
		return nil, fmt.Errorf("invalid latency budget %v set for analyzer %s", holdTime, info.Name)
	case holdTime == 0:
		holdTime = defaultLatencyBudgetMs
	}

	executorConfig := acpb.AnalyzerConfig_ExecutorConfig_builder{
		TimedEmitterConfig: acpb.AnalyzerConfig_ExecutorConfig_TimedEmitterConfig_builder{
			HoldTime: int32(holdTime),
		}.Build(),
	}.Build()
	return executorConfig, nil
}

func genAnalyzerConfig(info *asg.AnalyzerInfo, ctx *Context) (*acpb.AnalyzerConfig, error) {
	inputConfig, err := genInputConfig(info, ctx)
	if err != nil {
		return nil, err
	}

	executorConfig, err := genExecutorConfig(info, ctx)
	if err != nil {
		return nil, err
	}

	outputConfig, err := genOutputConfig(info, ctx)
	if err != nil {
		return nil, err
	}

	monitoringConfig := acpb.AnalyzerConfig_MonitoringConfig_builder{
		MetricsPort: int32(analyzerMetricsPort(info.MonitoringInfo)),
	}.Build()
	if analyzerMetricsDisabled(info.MonitoringInfo) {
		monitoringConfig = nil
	}

	analyzerConfig := acpb.AnalyzerConfig_builder{
		InputConfig: acpb.AnalyzerConfig_InputConfig_builder{
			InputReceivers: inputConfig,
		}.Build(),
		ExecutorConfig: executorConfig,
		OutputConfig: acpb.AnalyzerConfig_OutputConfig_builder{
			OutputSenders: outputConfig,
		}.Build(),
		MonitoringConfig: monitoringConfig,
	}.Build()
	if ctx.Verbose {
		title := fmt.Sprintf("AnalyzerConfig for %q", info.Name)
		fmt.Printf("\n%v\n", title)
		fmt.Printf("%v\n", strings.Repeat("=", len(title)))
		fmt.Printf("\n%v\n", prototext.Format(analyzerConfig))
	}
	return analyzerConfig, nil
}
