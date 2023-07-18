// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package k8s

import (
	"fmt"
	"strings"
	"time"

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

// Generates the input config specifically for "StreamSource" operators.
func genStreamSourceInputConfig(info *asg.AnalyzerInfo, ctx *Context) ([]*icpb.InputReceiverConfig, error) {
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

// Generates the input config specifically for "GcsVideoSource" operators.
func genGcsVideoSourceInputConfig(info *asg.AnalyzerInfo, ctx *Context) ([]*icpb.InputReceiverConfig, error) {
	inputVideoGcsPath := info.Attributes["input_video_gcs_path"]
	inputReceiverConfigs := []*icpb.InputReceiverConfig{
		icpb.InputReceiverConfig_builder{
			ChannelConfig: ccpb.InputChannelConfig_builder{
				GcsInputChannelConfig: ccpb.GcsInputChannelConfig_builder{
					InputVideoGcsPath: inputVideoGcsPath.Value.(string),
				}.Build(),
			}.Build(),
		}.Build(),
	}
	return inputReceiverConfigs, nil
}

// Generates the input config specifically for "WarehouseVideoSource" operators.
func genWarehouseVideoSourceInputConfig(info *asg.AnalyzerInfo, ctx *Context) ([]*icpb.InputReceiverConfig, error) {
	warehouseEndpoint := info.Attributes["warehouse_endpoint"]
	assetName := info.Attributes["asset_name"]
	fastMode := info.Attributes["fast_mode"]
	inputReceiverConfigs := []*icpb.InputReceiverConfig{
		icpb.InputReceiverConfig_builder{
			ChannelConfig: ccpb.InputChannelConfig_builder{
				WarehouseVideoInputChannelConfig: ccpb.WarehouseVideoInputChannelConfig_builder{
					WarehouseEndpoint:       warehouseEndpoint.Value.(string),
					WarehouseVideoAssetName: assetName.Value.(string),
					FastMode:                fastMode.Value.(bool),
				}.Build(),
			}.Build(),
		}.Build(),
	}
	return inputReceiverConfigs, nil
}

// Generates the input config for general operators.
// In this case, all inputs are grpc channels.
func genGeneralSourceInputConfig(info *asg.AnalyzerInfo, ctx *Context) ([]*icpb.InputReceiverConfig, error) {

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

func genInputConfig(info *asg.AnalyzerInfo, ctx *Context) ([]*icpb.InputReceiverConfig, error) {
	// TODO(b/271594223): We currently treat "StreamSource" and "GcsVideoSource"
	// in a special way. We will want to have this be itself customizable
	// through the operator framework so that we can handle this branch more
	// uniformly, rather than relying on specific operator triggers.
	switch info.Operator.Name {
	case "StreamSource":
		return genStreamSourceInputConfig(info, ctx)
	case "GcsVideoSource":
		return genGcsVideoSourceInputConfig(info, ctx)
	case "WarehouseVideoSource":
		return genWarehouseVideoSourceInputConfig(info, ctx)
	default:
		return genGeneralSourceInputConfig(info, ctx)
	}
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

func genGrpcOutputChannelConfig(streamInfo *asg.OutputStreamInfo) (*ccpb.OutputChannelConfig, error) {
	tokens := strings.Split(streamInfo.Stream.Name, ":")
	seriesName := tokens[1]
	outputChannelConfig := ccpb.OutputChannelConfig_builder{
		GrpcOutputChannelConfig: ccpb.GrpcOutputChannelConfig_builder{
			Series:                      seriesName,
			Peers:                       int32(streamInfo.Stream.DownstreamAnalyzers),
			DownstreamConnectionTimeout: int32(defaultDownstreamConnectionTimeout / time.Second),
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
		channelConfig, err := genGrpcOutputChannelConfig(streamInfo)
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

	runMode := acpb.AnalyzerConfig_RUN_MODE_UNSPECIFIED
	switch ctx.FeatureOptions.RunMode {
	case "live":
		runMode = acpb.AnalyzerConfig_LIVE
	case "submission":
		runMode = acpb.AnalyzerConfig_SUBMISSION
	default:
		return nil, fmt.Errorf("got an unsupported run mode %q", ctx.FeatureOptions.RunMode)
	}

	stateServerConfig := acpb.AnalyzerConfig_StateServerConfig_builder{
		Port: defaultStateServerPort,
	}.Build()

	adminServerConfig := acpb.AnalyzerConfig_AdminServerConfig_builder{
		Port:       defaultAdminServerPort,
		NumThreads: defaultAdminServerNumThreads,
	}.Build()

	analyzerConfig := acpb.AnalyzerConfig_builder{
		InputConfig: acpb.AnalyzerConfig_InputConfig_builder{
			InputReceivers: inputConfig,
		}.Build(),
		ExecutorConfig: executorConfig,
		OutputConfig: acpb.AnalyzerConfig_OutputConfig_builder{
			OutputSenders: outputConfig,
		}.Build(),
		MonitoringConfig:  monitoringConfig,
		RunMode:           runMode,
		StateServerConfig: stateServerConfig,
		AdminServerConfig: adminServerConfig,
	}.Build()
	if ctx.Verbose {
		title := fmt.Sprintf("AnalyzerConfig for %q", info.Name)
		fmt.Printf("\n%v\n", title)
		fmt.Printf("%v\n", strings.Repeat("=", len(title)))
		fmt.Printf("\n%v\n", prototext.Format(analyzerConfig))
	}
	return analyzerConfig, nil
}
