# -*- coding: utf-8 -*-
# Copyright 2023 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
import os
# try/except added for compatibility with python < 3.8
try:
    from unittest import mock
    from unittest.mock import AsyncMock  # pragma: NO COVER
except ImportError:  # pragma: NO COVER
    import mock

import grpc
from grpc.experimental import aio
from collections.abc import Iterable
from google.protobuf import json_format
import json
import math
import pytest
from proto.marshal.rules.dates import DurationRule, TimestampRule
from proto.marshal.rules import wrappers
from requests import Response
from requests import Request, PreparedRequest
from requests.sessions import Session
from google.protobuf import json_format

from google.api_core import client_options
from google.api_core import exceptions as core_exceptions
from google.api_core import future
from google.api_core import gapic_v1
from google.api_core import grpc_helpers
from google.api_core import grpc_helpers_async
from google.api_core import operation
from google.api_core import operation_async  # type: ignore
from google.api_core import operations_v1
from google.api_core import path_template
from google.auth import credentials as ga_credentials
from google.auth.exceptions import MutualTLSChannelError
from google.cloud.location import locations_pb2
from visionai.python.gapic.visionai.visionai_v1.services.live_video_analytics import LiveVideoAnalyticsAsyncClient
from visionai.python.gapic.visionai.visionai_v1.services.live_video_analytics import LiveVideoAnalyticsClient
from visionai.python.gapic.visionai.visionai_v1.services.live_video_analytics import pagers
from visionai.python.gapic.visionai.visionai_v1.services.live_video_analytics import transports
from visionai.python.gapic.visionai.visionai_v1.types import common
from visionai.python.gapic.visionai.visionai_v1.types import lva
from visionai.python.gapic.visionai.visionai_v1.types import lva_resources
from visionai.python.gapic.visionai.visionai_v1.types import lva_service
from google.iam.v1 import iam_policy_pb2  # type: ignore
from google.iam.v1 import options_pb2  # type: ignore
from google.iam.v1 import policy_pb2  # type: ignore
from google.longrunning import operations_pb2
from google.oauth2 import service_account
from google.protobuf import empty_pb2  # type: ignore
from google.protobuf import field_mask_pb2  # type: ignore
from google.protobuf import timestamp_pb2  # type: ignore
import google.auth


def client_cert_source_callback():
    return b"cert bytes", b"key bytes"


# If default endpoint is localhost, then default mtls endpoint will be the same.
# This method modifies the default endpoint so the client can produce a different
# mtls endpoint for endpoint testing purposes.
def modify_default_endpoint(client):
    return "foo.googleapis.com" if ("localhost" in client.DEFAULT_ENDPOINT) else client.DEFAULT_ENDPOINT


def test__get_default_mtls_endpoint():
    api_endpoint = "example.googleapis.com"
    api_mtls_endpoint = "example.mtls.googleapis.com"
    sandbox_endpoint = "example.sandbox.googleapis.com"
    sandbox_mtls_endpoint = "example.mtls.sandbox.googleapis.com"
    non_googleapi = "api.example.com"

    assert LiveVideoAnalyticsClient._get_default_mtls_endpoint(None) is None
    assert LiveVideoAnalyticsClient._get_default_mtls_endpoint(api_endpoint) == api_mtls_endpoint
    assert LiveVideoAnalyticsClient._get_default_mtls_endpoint(api_mtls_endpoint) == api_mtls_endpoint
    assert LiveVideoAnalyticsClient._get_default_mtls_endpoint(sandbox_endpoint) == sandbox_mtls_endpoint
    assert LiveVideoAnalyticsClient._get_default_mtls_endpoint(sandbox_mtls_endpoint) == sandbox_mtls_endpoint
    assert LiveVideoAnalyticsClient._get_default_mtls_endpoint(non_googleapi) == non_googleapi


@pytest.mark.parametrize("client_class,transport_name", [
    (LiveVideoAnalyticsClient, "grpc"),
    (LiveVideoAnalyticsAsyncClient, "grpc_asyncio"),
    (LiveVideoAnalyticsClient, "rest"),
])
def test_live_video_analytics_client_from_service_account_info(client_class, transport_name):
    creds = ga_credentials.AnonymousCredentials()
    with mock.patch.object(service_account.Credentials, 'from_service_account_info') as factory:
        factory.return_value = creds
        info = {"valid": True}
        client = client_class.from_service_account_info(info, transport=transport_name)
        assert client.transport._credentials == creds
        assert isinstance(client, client_class)

        assert client.transport._host == (
            'visionai.googleapis.com:443'
            if transport_name in ['grpc', 'grpc_asyncio']
            else
            'https://visionai.googleapis.com'
        )


@pytest.mark.parametrize("transport_class,transport_name", [
    (transports.LiveVideoAnalyticsGrpcTransport, "grpc"),
    (transports.LiveVideoAnalyticsGrpcAsyncIOTransport, "grpc_asyncio"),
    (transports.LiveVideoAnalyticsRestTransport, "rest"),
])
def test_live_video_analytics_client_service_account_always_use_jwt(transport_class, transport_name):
    with mock.patch.object(service_account.Credentials, 'with_always_use_jwt_access', create=True) as use_jwt:
        creds = service_account.Credentials(None, None, None)
        transport = transport_class(credentials=creds, always_use_jwt_access=True)
        use_jwt.assert_called_once_with(True)

    with mock.patch.object(service_account.Credentials, 'with_always_use_jwt_access', create=True) as use_jwt:
        creds = service_account.Credentials(None, None, None)
        transport = transport_class(credentials=creds, always_use_jwt_access=False)
        use_jwt.assert_not_called()


@pytest.mark.parametrize("client_class,transport_name", [
    (LiveVideoAnalyticsClient, "grpc"),
    (LiveVideoAnalyticsAsyncClient, "grpc_asyncio"),
    (LiveVideoAnalyticsClient, "rest"),
])
def test_live_video_analytics_client_from_service_account_file(client_class, transport_name):
    creds = ga_credentials.AnonymousCredentials()
    with mock.patch.object(service_account.Credentials, 'from_service_account_file') as factory:
        factory.return_value = creds
        client = client_class.from_service_account_file("dummy/file/path.json", transport=transport_name)
        assert client.transport._credentials == creds
        assert isinstance(client, client_class)

        client = client_class.from_service_account_json("dummy/file/path.json", transport=transport_name)
        assert client.transport._credentials == creds
        assert isinstance(client, client_class)

        assert client.transport._host == (
            'visionai.googleapis.com:443'
            if transport_name in ['grpc', 'grpc_asyncio']
            else
            'https://visionai.googleapis.com'
        )


def test_live_video_analytics_client_get_transport_class():
    transport = LiveVideoAnalyticsClient.get_transport_class()
    available_transports = [
        transports.LiveVideoAnalyticsGrpcTransport,
        transports.LiveVideoAnalyticsRestTransport,
    ]
    assert transport in available_transports

    transport = LiveVideoAnalyticsClient.get_transport_class("grpc")
    assert transport == transports.LiveVideoAnalyticsGrpcTransport


@pytest.mark.parametrize("client_class,transport_class,transport_name", [
    (LiveVideoAnalyticsClient, transports.LiveVideoAnalyticsGrpcTransport, "grpc"),
    (LiveVideoAnalyticsAsyncClient, transports.LiveVideoAnalyticsGrpcAsyncIOTransport, "grpc_asyncio"),
    (LiveVideoAnalyticsClient, transports.LiveVideoAnalyticsRestTransport, "rest"),
])
@mock.patch.object(LiveVideoAnalyticsClient, "DEFAULT_ENDPOINT", modify_default_endpoint(LiveVideoAnalyticsClient))
@mock.patch.object(LiveVideoAnalyticsAsyncClient, "DEFAULT_ENDPOINT", modify_default_endpoint(LiveVideoAnalyticsAsyncClient))
def test_live_video_analytics_client_client_options(client_class, transport_class, transport_name):
    # Check that if channel is provided we won't create a new one.
    with mock.patch.object(LiveVideoAnalyticsClient, 'get_transport_class') as gtc:
        transport = transport_class(
            credentials=ga_credentials.AnonymousCredentials()
        )
        client = client_class(transport=transport)
        gtc.assert_not_called()

    # Check that if channel is provided via str we will create a new one.
    with mock.patch.object(LiveVideoAnalyticsClient, 'get_transport_class') as gtc:
        client = client_class(transport=transport_name)
        gtc.assert_called()

    # Check the case api_endpoint is provided.
    options = client_options.ClientOptions(api_endpoint="squid.clam.whelk")
    with mock.patch.object(transport_class, '__init__') as patched:
        patched.return_value = None
        client = client_class(transport=transport_name, client_options=options)
        patched.assert_called_once_with(
            credentials=None,
            credentials_file=None,
            host="squid.clam.whelk",
            scopes=None,
            client_cert_source_for_mtls=None,
            quota_project_id=None,
            client_info=transports.base.DEFAULT_CLIENT_INFO,
            always_use_jwt_access=True,
            api_audience=None,
        )

    # Check the case api_endpoint is not provided and GOOGLE_API_USE_MTLS_ENDPOINT is
    # "never".
    with mock.patch.dict(os.environ, {"GOOGLE_API_USE_MTLS_ENDPOINT": "never"}):
        with mock.patch.object(transport_class, '__init__') as patched:
            patched.return_value = None
            client = client_class(transport=transport_name)
            patched.assert_called_once_with(
                credentials=None,
                credentials_file=None,
                host=client.DEFAULT_ENDPOINT,
                scopes=None,
                client_cert_source_for_mtls=None,
                quota_project_id=None,
                client_info=transports.base.DEFAULT_CLIENT_INFO,
                always_use_jwt_access=True,
                api_audience=None,
            )

    # Check the case api_endpoint is not provided and GOOGLE_API_USE_MTLS_ENDPOINT is
    # "always".
    with mock.patch.dict(os.environ, {"GOOGLE_API_USE_MTLS_ENDPOINT": "always"}):
        with mock.patch.object(transport_class, '__init__') as patched:
            patched.return_value = None
            client = client_class(transport=transport_name)
            patched.assert_called_once_with(
                credentials=None,
                credentials_file=None,
                host=client.DEFAULT_MTLS_ENDPOINT,
                scopes=None,
                client_cert_source_for_mtls=None,
                quota_project_id=None,
                client_info=transports.base.DEFAULT_CLIENT_INFO,
                always_use_jwt_access=True,
                api_audience=None,
            )

    # Check the case api_endpoint is not provided and GOOGLE_API_USE_MTLS_ENDPOINT has
    # unsupported value.
    with mock.patch.dict(os.environ, {"GOOGLE_API_USE_MTLS_ENDPOINT": "Unsupported"}):
        with pytest.raises(MutualTLSChannelError):
            client = client_class(transport=transport_name)

    # Check the case GOOGLE_API_USE_CLIENT_CERTIFICATE has unsupported value.
    with mock.patch.dict(os.environ, {"GOOGLE_API_USE_CLIENT_CERTIFICATE": "Unsupported"}):
        with pytest.raises(ValueError):
            client = client_class(transport=transport_name)

    # Check the case quota_project_id is provided
    options = client_options.ClientOptions(quota_project_id="octopus")
    with mock.patch.object(transport_class, '__init__') as patched:
        patched.return_value = None
        client = client_class(client_options=options, transport=transport_name)
        patched.assert_called_once_with(
            credentials=None,
            credentials_file=None,
            host=client.DEFAULT_ENDPOINT,
            scopes=None,
            client_cert_source_for_mtls=None,
            quota_project_id="octopus",
            client_info=transports.base.DEFAULT_CLIENT_INFO,
            always_use_jwt_access=True,
            api_audience=None,
        )
    # Check the case api_endpoint is provided
    options = client_options.ClientOptions(api_audience="https://language.googleapis.com")
    with mock.patch.object(transport_class, '__init__') as patched:
        patched.return_value = None
        client = client_class(client_options=options, transport=transport_name)
        patched.assert_called_once_with(
            credentials=None,
            credentials_file=None,
            host=client.DEFAULT_ENDPOINT,
            scopes=None,
            client_cert_source_for_mtls=None,
            quota_project_id=None,
            client_info=transports.base.DEFAULT_CLIENT_INFO,
            always_use_jwt_access=True,
            api_audience="https://language.googleapis.com"
        )

@pytest.mark.parametrize("client_class,transport_class,transport_name,use_client_cert_env", [
    (LiveVideoAnalyticsClient, transports.LiveVideoAnalyticsGrpcTransport, "grpc", "true"),
    (LiveVideoAnalyticsAsyncClient, transports.LiveVideoAnalyticsGrpcAsyncIOTransport, "grpc_asyncio", "true"),
    (LiveVideoAnalyticsClient, transports.LiveVideoAnalyticsGrpcTransport, "grpc", "false"),
    (LiveVideoAnalyticsAsyncClient, transports.LiveVideoAnalyticsGrpcAsyncIOTransport, "grpc_asyncio", "false"),
    (LiveVideoAnalyticsClient, transports.LiveVideoAnalyticsRestTransport, "rest", "true"),
    (LiveVideoAnalyticsClient, transports.LiveVideoAnalyticsRestTransport, "rest", "false"),
])
@mock.patch.object(LiveVideoAnalyticsClient, "DEFAULT_ENDPOINT", modify_default_endpoint(LiveVideoAnalyticsClient))
@mock.patch.object(LiveVideoAnalyticsAsyncClient, "DEFAULT_ENDPOINT", modify_default_endpoint(LiveVideoAnalyticsAsyncClient))
@mock.patch.dict(os.environ, {"GOOGLE_API_USE_MTLS_ENDPOINT": "auto"})
def test_live_video_analytics_client_mtls_env_auto(client_class, transport_class, transport_name, use_client_cert_env):
    # This tests the endpoint autoswitch behavior. Endpoint is autoswitched to the default
    # mtls endpoint, if GOOGLE_API_USE_CLIENT_CERTIFICATE is "true" and client cert exists.

    # Check the case client_cert_source is provided. Whether client cert is used depends on
    # GOOGLE_API_USE_CLIENT_CERTIFICATE value.
    with mock.patch.dict(os.environ, {"GOOGLE_API_USE_CLIENT_CERTIFICATE": use_client_cert_env}):
        options = client_options.ClientOptions(client_cert_source=client_cert_source_callback)
        with mock.patch.object(transport_class, '__init__') as patched:
            patched.return_value = None
            client = client_class(client_options=options, transport=transport_name)

            if use_client_cert_env == "false":
                expected_client_cert_source = None
                expected_host = client.DEFAULT_ENDPOINT
            else:
                expected_client_cert_source = client_cert_source_callback
                expected_host = client.DEFAULT_MTLS_ENDPOINT

            patched.assert_called_once_with(
                credentials=None,
                credentials_file=None,
                host=expected_host,
                scopes=None,
                client_cert_source_for_mtls=expected_client_cert_source,
                quota_project_id=None,
                client_info=transports.base.DEFAULT_CLIENT_INFO,
                always_use_jwt_access=True,
                api_audience=None,
            )

    # Check the case ADC client cert is provided. Whether client cert is used depends on
    # GOOGLE_API_USE_CLIENT_CERTIFICATE value.
    with mock.patch.dict(os.environ, {"GOOGLE_API_USE_CLIENT_CERTIFICATE": use_client_cert_env}):
        with mock.patch.object(transport_class, '__init__') as patched:
            with mock.patch('google.auth.transport.mtls.has_default_client_cert_source', return_value=True):
                with mock.patch('google.auth.transport.mtls.default_client_cert_source', return_value=client_cert_source_callback):
                    if use_client_cert_env == "false":
                        expected_host = client.DEFAULT_ENDPOINT
                        expected_client_cert_source = None
                    else:
                        expected_host = client.DEFAULT_MTLS_ENDPOINT
                        expected_client_cert_source = client_cert_source_callback

                    patched.return_value = None
                    client = client_class(transport=transport_name)
                    patched.assert_called_once_with(
                        credentials=None,
                        credentials_file=None,
                        host=expected_host,
                        scopes=None,
                        client_cert_source_for_mtls=expected_client_cert_source,
                        quota_project_id=None,
                        client_info=transports.base.DEFAULT_CLIENT_INFO,
                        always_use_jwt_access=True,
                        api_audience=None,
                    )

    # Check the case client_cert_source and ADC client cert are not provided.
    with mock.patch.dict(os.environ, {"GOOGLE_API_USE_CLIENT_CERTIFICATE": use_client_cert_env}):
        with mock.patch.object(transport_class, '__init__') as patched:
            with mock.patch("google.auth.transport.mtls.has_default_client_cert_source", return_value=False):
                patched.return_value = None
                client = client_class(transport=transport_name)
                patched.assert_called_once_with(
                    credentials=None,
                    credentials_file=None,
                    host=client.DEFAULT_ENDPOINT,
                    scopes=None,
                    client_cert_source_for_mtls=None,
                    quota_project_id=None,
                    client_info=transports.base.DEFAULT_CLIENT_INFO,
                    always_use_jwt_access=True,
                    api_audience=None,
                )


@pytest.mark.parametrize("client_class", [
    LiveVideoAnalyticsClient, LiveVideoAnalyticsAsyncClient
])
@mock.patch.object(LiveVideoAnalyticsClient, "DEFAULT_ENDPOINT", modify_default_endpoint(LiveVideoAnalyticsClient))
@mock.patch.object(LiveVideoAnalyticsAsyncClient, "DEFAULT_ENDPOINT", modify_default_endpoint(LiveVideoAnalyticsAsyncClient))
def test_live_video_analytics_client_get_mtls_endpoint_and_cert_source(client_class):
    mock_client_cert_source = mock.Mock()

    # Test the case GOOGLE_API_USE_CLIENT_CERTIFICATE is "true".
    with mock.patch.dict(os.environ, {"GOOGLE_API_USE_CLIENT_CERTIFICATE": "true"}):
        mock_api_endpoint = "foo"
        options = client_options.ClientOptions(client_cert_source=mock_client_cert_source, api_endpoint=mock_api_endpoint)
        api_endpoint, cert_source = client_class.get_mtls_endpoint_and_cert_source(options)
        assert api_endpoint == mock_api_endpoint
        assert cert_source == mock_client_cert_source

    # Test the case GOOGLE_API_USE_CLIENT_CERTIFICATE is "false".
    with mock.patch.dict(os.environ, {"GOOGLE_API_USE_CLIENT_CERTIFICATE": "false"}):
        mock_client_cert_source = mock.Mock()
        mock_api_endpoint = "foo"
        options = client_options.ClientOptions(client_cert_source=mock_client_cert_source, api_endpoint=mock_api_endpoint)
        api_endpoint, cert_source = client_class.get_mtls_endpoint_and_cert_source(options)
        assert api_endpoint == mock_api_endpoint
        assert cert_source is None

    # Test the case GOOGLE_API_USE_MTLS_ENDPOINT is "never".
    with mock.patch.dict(os.environ, {"GOOGLE_API_USE_MTLS_ENDPOINT": "never"}):
        api_endpoint, cert_source = client_class.get_mtls_endpoint_and_cert_source()
        assert api_endpoint == client_class.DEFAULT_ENDPOINT
        assert cert_source is None

    # Test the case GOOGLE_API_USE_MTLS_ENDPOINT is "always".
    with mock.patch.dict(os.environ, {"GOOGLE_API_USE_MTLS_ENDPOINT": "always"}):
        api_endpoint, cert_source = client_class.get_mtls_endpoint_and_cert_source()
        assert api_endpoint == client_class.DEFAULT_MTLS_ENDPOINT
        assert cert_source is None

    # Test the case GOOGLE_API_USE_MTLS_ENDPOINT is "auto" and default cert doesn't exist.
    with mock.patch.dict(os.environ, {"GOOGLE_API_USE_CLIENT_CERTIFICATE": "true"}):
        with mock.patch('google.auth.transport.mtls.has_default_client_cert_source', return_value=False):
            api_endpoint, cert_source = client_class.get_mtls_endpoint_and_cert_source()
            assert api_endpoint == client_class.DEFAULT_ENDPOINT
            assert cert_source is None

    # Test the case GOOGLE_API_USE_MTLS_ENDPOINT is "auto" and default cert exists.
    with mock.patch.dict(os.environ, {"GOOGLE_API_USE_CLIENT_CERTIFICATE": "true"}):
        with mock.patch('google.auth.transport.mtls.has_default_client_cert_source', return_value=True):
            with mock.patch('google.auth.transport.mtls.default_client_cert_source', return_value=mock_client_cert_source):
                api_endpoint, cert_source = client_class.get_mtls_endpoint_and_cert_source()
                assert api_endpoint == client_class.DEFAULT_MTLS_ENDPOINT
                assert cert_source == mock_client_cert_source


@pytest.mark.parametrize("client_class,transport_class,transport_name", [
    (LiveVideoAnalyticsClient, transports.LiveVideoAnalyticsGrpcTransport, "grpc"),
    (LiveVideoAnalyticsAsyncClient, transports.LiveVideoAnalyticsGrpcAsyncIOTransport, "grpc_asyncio"),
    (LiveVideoAnalyticsClient, transports.LiveVideoAnalyticsRestTransport, "rest"),
])
def test_live_video_analytics_client_client_options_scopes(client_class, transport_class, transport_name):
    # Check the case scopes are provided.
    options = client_options.ClientOptions(
        scopes=["1", "2"],
    )
    with mock.patch.object(transport_class, '__init__') as patched:
        patched.return_value = None
        client = client_class(client_options=options, transport=transport_name)
        patched.assert_called_once_with(
            credentials=None,
            credentials_file=None,
            host=client.DEFAULT_ENDPOINT,
            scopes=["1", "2"],
            client_cert_source_for_mtls=None,
            quota_project_id=None,
            client_info=transports.base.DEFAULT_CLIENT_INFO,
            always_use_jwt_access=True,
            api_audience=None,
        )

@pytest.mark.parametrize("client_class,transport_class,transport_name,grpc_helpers", [
    (LiveVideoAnalyticsClient, transports.LiveVideoAnalyticsGrpcTransport, "grpc", grpc_helpers),
    (LiveVideoAnalyticsAsyncClient, transports.LiveVideoAnalyticsGrpcAsyncIOTransport, "grpc_asyncio", grpc_helpers_async),
    (LiveVideoAnalyticsClient, transports.LiveVideoAnalyticsRestTransport, "rest", None),
])
def test_live_video_analytics_client_client_options_credentials_file(client_class, transport_class, transport_name, grpc_helpers):
    # Check the case credentials file is provided.
    options = client_options.ClientOptions(
        credentials_file="credentials.json"
    )

    with mock.patch.object(transport_class, '__init__') as patched:
        patched.return_value = None
        client = client_class(client_options=options, transport=transport_name)
        patched.assert_called_once_with(
            credentials=None,
            credentials_file="credentials.json",
            host=client.DEFAULT_ENDPOINT,
            scopes=None,
            client_cert_source_for_mtls=None,
            quota_project_id=None,
            client_info=transports.base.DEFAULT_CLIENT_INFO,
            always_use_jwt_access=True,
            api_audience=None,
        )

def test_live_video_analytics_client_client_options_from_dict():
    with mock.patch('visionai.python.gapic.visionai.visionai_v1.services.live_video_analytics.transports.LiveVideoAnalyticsGrpcTransport.__init__') as grpc_transport:
        grpc_transport.return_value = None
        client = LiveVideoAnalyticsClient(
            client_options={'api_endpoint': 'squid.clam.whelk'}
        )
        grpc_transport.assert_called_once_with(
            credentials=None,
            credentials_file=None,
            host="squid.clam.whelk",
            scopes=None,
            client_cert_source_for_mtls=None,
            quota_project_id=None,
            client_info=transports.base.DEFAULT_CLIENT_INFO,
            always_use_jwt_access=True,
            api_audience=None,
        )


@pytest.mark.parametrize("client_class,transport_class,transport_name,grpc_helpers", [
    (LiveVideoAnalyticsClient, transports.LiveVideoAnalyticsGrpcTransport, "grpc", grpc_helpers),
    (LiveVideoAnalyticsAsyncClient, transports.LiveVideoAnalyticsGrpcAsyncIOTransport, "grpc_asyncio", grpc_helpers_async),
])
def test_live_video_analytics_client_create_channel_credentials_file(client_class, transport_class, transport_name, grpc_helpers):
    # Check the case credentials file is provided.
    options = client_options.ClientOptions(
        credentials_file="credentials.json"
    )

    with mock.patch.object(transport_class, '__init__') as patched:
        patched.return_value = None
        client = client_class(client_options=options, transport=transport_name)
        patched.assert_called_once_with(
            credentials=None,
            credentials_file="credentials.json",
            host=client.DEFAULT_ENDPOINT,
            scopes=None,
            client_cert_source_for_mtls=None,
            quota_project_id=None,
            client_info=transports.base.DEFAULT_CLIENT_INFO,
            always_use_jwt_access=True,
            api_audience=None,
        )

    # test that the credentials from file are saved and used as the credentials.
    with mock.patch.object(
        google.auth, "load_credentials_from_file", autospec=True
    ) as load_creds, mock.patch.object(
        google.auth, "default", autospec=True
    ) as adc, mock.patch.object(
        grpc_helpers, "create_channel"
    ) as create_channel:
        creds = ga_credentials.AnonymousCredentials()
        file_creds = ga_credentials.AnonymousCredentials()
        load_creds.return_value = (file_creds, None)
        adc.return_value = (creds, None)
        client = client_class(client_options=options, transport=transport_name)
        create_channel.assert_called_with(
            "visionai.googleapis.com:443",
            credentials=file_creds,
            credentials_file=None,
            quota_project_id=None,
            default_scopes=(
                'https://www.googleapis.com/auth/cloud-platform',
),
            scopes=None,
            default_host="visionai.googleapis.com",
            ssl_credentials=None,
            options=[
                ("grpc.max_send_message_length", -1),
                ("grpc.max_receive_message_length", -1),
            ],
        )


@pytest.mark.parametrize("request_type", [
  lva_service.ResolveOperatorInfoRequest,
  dict,
])
def test_resolve_operator_info(request_type, transport: str = 'grpc'):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # Everything is optional in proto3 as far as the runtime is concerned,
    # and we are mocking out the actual API, so just send an empty request.
    request = request_type()

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.resolve_operator_info),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = lva_service.ResolveOperatorInfoResponse(
        )
        response = client.resolve_operator_info(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        assert args[0] == lva_service.ResolveOperatorInfoRequest()

    # Establish that the response is the type that we expect.
    assert isinstance(response, lva_service.ResolveOperatorInfoResponse)


def test_resolve_operator_info_empty_call():
    # This test is a coverage failsafe to make sure that totally empty calls,
    # i.e. request == None and no flattened fields passed, work.
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport='grpc',
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.resolve_operator_info),
            '__call__') as call:
        client.resolve_operator_info()
        call.assert_called()
        _, args, _ = call.mock_calls[0]
        assert args[0] == lva_service.ResolveOperatorInfoRequest()

@pytest.mark.asyncio
async def test_resolve_operator_info_async(transport: str = 'grpc_asyncio', request_type=lva_service.ResolveOperatorInfoRequest):
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # Everything is optional in proto3 as far as the runtime is concerned,
    # and we are mocking out the actual API, so just send an empty request.
    request = request_type()

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.resolve_operator_info),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value =grpc_helpers_async.FakeUnaryUnaryCall(lva_service.ResolveOperatorInfoResponse(
        ))
        response = await client.resolve_operator_info(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls)
        _, args, _ = call.mock_calls[0]
        assert args[0] == lva_service.ResolveOperatorInfoRequest()

    # Establish that the response is the type that we expect.
    assert isinstance(response, lva_service.ResolveOperatorInfoResponse)


@pytest.mark.asyncio
async def test_resolve_operator_info_async_from_dict():
    await test_resolve_operator_info_async(request_type=dict)


def test_resolve_operator_info_field_headers():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Any value that is part of the HTTP/1.1 URI should be sent as
    # a field header. Set these to a non-empty value.
    request = lva_service.ResolveOperatorInfoRequest()

    request.parent = 'parent_value'

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.resolve_operator_info),
            '__call__') as call:
        call.return_value = lva_service.ResolveOperatorInfoResponse()
        client.resolve_operator_info(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        assert args[0] == request

    # Establish that the field header was sent.
    _, _, kw = call.mock_calls[0]
    assert (
        'x-goog-request-params',
        'parent=parent_value',
    ) in kw['metadata']


@pytest.mark.asyncio
async def test_resolve_operator_info_field_headers_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Any value that is part of the HTTP/1.1 URI should be sent as
    # a field header. Set these to a non-empty value.
    request = lva_service.ResolveOperatorInfoRequest()

    request.parent = 'parent_value'

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.resolve_operator_info),
            '__call__') as call:
        call.return_value = grpc_helpers_async.FakeUnaryUnaryCall(lva_service.ResolveOperatorInfoResponse())
        await client.resolve_operator_info(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls)
        _, args, _ = call.mock_calls[0]
        assert args[0] == request

    # Establish that the field header was sent.
    _, _, kw = call.mock_calls[0]
    assert (
        'x-goog-request-params',
        'parent=parent_value',
    ) in kw['metadata']


def test_resolve_operator_info_flattened():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.resolve_operator_info),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = lva_service.ResolveOperatorInfoResponse()
        # Call the method with a truthy value for each flattened field,
        # using the keyword arguments to the method.
        client.resolve_operator_info(
            parent='parent_value',
            queries=[lva_service.OperatorQuery(operator='operator_value')],
        )

        # Establish that the underlying call was made with the expected
        # request object values.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        arg = args[0].parent
        mock_val = 'parent_value'
        assert arg == mock_val
        arg = args[0].queries
        mock_val = [lva_service.OperatorQuery(operator='operator_value')]
        assert arg == mock_val


def test_resolve_operator_info_flattened_error():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Attempting to call a method with both a request object and flattened
    # fields is an error.
    with pytest.raises(ValueError):
        client.resolve_operator_info(
            lva_service.ResolveOperatorInfoRequest(),
            parent='parent_value',
            queries=[lva_service.OperatorQuery(operator='operator_value')],
        )

@pytest.mark.asyncio
async def test_resolve_operator_info_flattened_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.resolve_operator_info),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = lva_service.ResolveOperatorInfoResponse()

        call.return_value = grpc_helpers_async.FakeUnaryUnaryCall(lva_service.ResolveOperatorInfoResponse())
        # Call the method with a truthy value for each flattened field,
        # using the keyword arguments to the method.
        response = await client.resolve_operator_info(
            parent='parent_value',
            queries=[lva_service.OperatorQuery(operator='operator_value')],
        )

        # Establish that the underlying call was made with the expected
        # request object values.
        assert len(call.mock_calls)
        _, args, _ = call.mock_calls[0]
        arg = args[0].parent
        mock_val = 'parent_value'
        assert arg == mock_val
        arg = args[0].queries
        mock_val = [lva_service.OperatorQuery(operator='operator_value')]
        assert arg == mock_val

@pytest.mark.asyncio
async def test_resolve_operator_info_flattened_error_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Attempting to call a method with both a request object and flattened
    # fields is an error.
    with pytest.raises(ValueError):
        await client.resolve_operator_info(
            lva_service.ResolveOperatorInfoRequest(),
            parent='parent_value',
            queries=[lva_service.OperatorQuery(operator='operator_value')],
        )


@pytest.mark.parametrize("request_type", [
  lva_service.ListOperatorsRequest,
  dict,
])
def test_list_operators(request_type, transport: str = 'grpc'):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # Everything is optional in proto3 as far as the runtime is concerned,
    # and we are mocking out the actual API, so just send an empty request.
    request = request_type()

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.list_operators),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = lva_service.ListOperatorsResponse(
            next_page_token='next_page_token_value',
            unreachable=['unreachable_value'],
        )
        response = client.list_operators(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        assert args[0] == lva_service.ListOperatorsRequest()

    # Establish that the response is the type that we expect.
    assert isinstance(response, pagers.ListOperatorsPager)
    assert response.next_page_token == 'next_page_token_value'
    assert response.unreachable == ['unreachable_value']


def test_list_operators_empty_call():
    # This test is a coverage failsafe to make sure that totally empty calls,
    # i.e. request == None and no flattened fields passed, work.
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport='grpc',
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.list_operators),
            '__call__') as call:
        client.list_operators()
        call.assert_called()
        _, args, _ = call.mock_calls[0]
        assert args[0] == lva_service.ListOperatorsRequest()

@pytest.mark.asyncio
async def test_list_operators_async(transport: str = 'grpc_asyncio', request_type=lva_service.ListOperatorsRequest):
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # Everything is optional in proto3 as far as the runtime is concerned,
    # and we are mocking out the actual API, so just send an empty request.
    request = request_type()

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.list_operators),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value =grpc_helpers_async.FakeUnaryUnaryCall(lva_service.ListOperatorsResponse(
            next_page_token='next_page_token_value',
            unreachable=['unreachable_value'],
        ))
        response = await client.list_operators(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls)
        _, args, _ = call.mock_calls[0]
        assert args[0] == lva_service.ListOperatorsRequest()

    # Establish that the response is the type that we expect.
    assert isinstance(response, pagers.ListOperatorsAsyncPager)
    assert response.next_page_token == 'next_page_token_value'
    assert response.unreachable == ['unreachable_value']


@pytest.mark.asyncio
async def test_list_operators_async_from_dict():
    await test_list_operators_async(request_type=dict)


def test_list_operators_field_headers():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Any value that is part of the HTTP/1.1 URI should be sent as
    # a field header. Set these to a non-empty value.
    request = lva_service.ListOperatorsRequest()

    request.parent = 'parent_value'

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.list_operators),
            '__call__') as call:
        call.return_value = lva_service.ListOperatorsResponse()
        client.list_operators(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        assert args[0] == request

    # Establish that the field header was sent.
    _, _, kw = call.mock_calls[0]
    assert (
        'x-goog-request-params',
        'parent=parent_value',
    ) in kw['metadata']


@pytest.mark.asyncio
async def test_list_operators_field_headers_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Any value that is part of the HTTP/1.1 URI should be sent as
    # a field header. Set these to a non-empty value.
    request = lva_service.ListOperatorsRequest()

    request.parent = 'parent_value'

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.list_operators),
            '__call__') as call:
        call.return_value = grpc_helpers_async.FakeUnaryUnaryCall(lva_service.ListOperatorsResponse())
        await client.list_operators(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls)
        _, args, _ = call.mock_calls[0]
        assert args[0] == request

    # Establish that the field header was sent.
    _, _, kw = call.mock_calls[0]
    assert (
        'x-goog-request-params',
        'parent=parent_value',
    ) in kw['metadata']


def test_list_operators_flattened():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.list_operators),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = lva_service.ListOperatorsResponse()
        # Call the method with a truthy value for each flattened field,
        # using the keyword arguments to the method.
        client.list_operators(
            parent='parent_value',
        )

        # Establish that the underlying call was made with the expected
        # request object values.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        arg = args[0].parent
        mock_val = 'parent_value'
        assert arg == mock_val


def test_list_operators_flattened_error():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Attempting to call a method with both a request object and flattened
    # fields is an error.
    with pytest.raises(ValueError):
        client.list_operators(
            lva_service.ListOperatorsRequest(),
            parent='parent_value',
        )

@pytest.mark.asyncio
async def test_list_operators_flattened_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.list_operators),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = lva_service.ListOperatorsResponse()

        call.return_value = grpc_helpers_async.FakeUnaryUnaryCall(lva_service.ListOperatorsResponse())
        # Call the method with a truthy value for each flattened field,
        # using the keyword arguments to the method.
        response = await client.list_operators(
            parent='parent_value',
        )

        # Establish that the underlying call was made with the expected
        # request object values.
        assert len(call.mock_calls)
        _, args, _ = call.mock_calls[0]
        arg = args[0].parent
        mock_val = 'parent_value'
        assert arg == mock_val

@pytest.mark.asyncio
async def test_list_operators_flattened_error_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Attempting to call a method with both a request object and flattened
    # fields is an error.
    with pytest.raises(ValueError):
        await client.list_operators(
            lva_service.ListOperatorsRequest(),
            parent='parent_value',
        )


def test_list_operators_pager(transport_name: str = "grpc"):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials,
        transport=transport_name,
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.list_operators),
            '__call__') as call:
        # Set the response to a series of pages.
        call.side_effect = (
            lva_service.ListOperatorsResponse(
                operators=[
                    lva_resources.Operator(),
                    lva_resources.Operator(),
                    lva_resources.Operator(),
                ],
                next_page_token='abc',
            ),
            lva_service.ListOperatorsResponse(
                operators=[],
                next_page_token='def',
            ),
            lva_service.ListOperatorsResponse(
                operators=[
                    lva_resources.Operator(),
                ],
                next_page_token='ghi',
            ),
            lva_service.ListOperatorsResponse(
                operators=[
                    lva_resources.Operator(),
                    lva_resources.Operator(),
                ],
            ),
            RuntimeError,
        )

        metadata = ()
        metadata = tuple(metadata) + (
            gapic_v1.routing_header.to_grpc_metadata((
                ('parent', ''),
            )),
        )
        pager = client.list_operators(request={})

        assert pager._metadata == metadata

        results = list(pager)
        assert len(results) == 6
        assert all(isinstance(i, lva_resources.Operator)
                   for i in results)
def test_list_operators_pages(transport_name: str = "grpc"):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials,
        transport=transport_name,
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.list_operators),
            '__call__') as call:
        # Set the response to a series of pages.
        call.side_effect = (
            lva_service.ListOperatorsResponse(
                operators=[
                    lva_resources.Operator(),
                    lva_resources.Operator(),
                    lva_resources.Operator(),
                ],
                next_page_token='abc',
            ),
            lva_service.ListOperatorsResponse(
                operators=[],
                next_page_token='def',
            ),
            lva_service.ListOperatorsResponse(
                operators=[
                    lva_resources.Operator(),
                ],
                next_page_token='ghi',
            ),
            lva_service.ListOperatorsResponse(
                operators=[
                    lva_resources.Operator(),
                    lva_resources.Operator(),
                ],
            ),
            RuntimeError,
        )
        pages = list(client.list_operators(request={}).pages)
        for page_, token in zip(pages, ['abc','def','ghi', '']):
            assert page_.raw_page.next_page_token == token

@pytest.mark.asyncio
async def test_list_operators_async_pager():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials,
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.list_operators),
            '__call__', new_callable=mock.AsyncMock) as call:
        # Set the response to a series of pages.
        call.side_effect = (
            lva_service.ListOperatorsResponse(
                operators=[
                    lva_resources.Operator(),
                    lva_resources.Operator(),
                    lva_resources.Operator(),
                ],
                next_page_token='abc',
            ),
            lva_service.ListOperatorsResponse(
                operators=[],
                next_page_token='def',
            ),
            lva_service.ListOperatorsResponse(
                operators=[
                    lva_resources.Operator(),
                ],
                next_page_token='ghi',
            ),
            lva_service.ListOperatorsResponse(
                operators=[
                    lva_resources.Operator(),
                    lva_resources.Operator(),
                ],
            ),
            RuntimeError,
        )
        async_pager = await client.list_operators(request={},)
        assert async_pager.next_page_token == 'abc'
        responses = []
        async for response in async_pager: # pragma: no branch
            responses.append(response)

        assert len(responses) == 6
        assert all(isinstance(i, lva_resources.Operator)
                for i in responses)


@pytest.mark.asyncio
async def test_list_operators_async_pages():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials,
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.list_operators),
            '__call__', new_callable=mock.AsyncMock) as call:
        # Set the response to a series of pages.
        call.side_effect = (
            lva_service.ListOperatorsResponse(
                operators=[
                    lva_resources.Operator(),
                    lva_resources.Operator(),
                    lva_resources.Operator(),
                ],
                next_page_token='abc',
            ),
            lva_service.ListOperatorsResponse(
                operators=[],
                next_page_token='def',
            ),
            lva_service.ListOperatorsResponse(
                operators=[
                    lva_resources.Operator(),
                ],
                next_page_token='ghi',
            ),
            lva_service.ListOperatorsResponse(
                operators=[
                    lva_resources.Operator(),
                    lva_resources.Operator(),
                ],
            ),
            RuntimeError,
        )
        pages = []
        # Workaround issue in python 3.9 related to code coverage by adding `# pragma: no branch`
        # See https://github.com/googleapis/gapic-generator-python/pull/1174#issuecomment-1025132372
        async for page_ in ( # pragma: no branch
            await client.list_operators(request={})
        ).pages:
            pages.append(page_)
        for page_, token in zip(pages, ['abc','def','ghi', '']):
            assert page_.raw_page.next_page_token == token

@pytest.mark.parametrize("request_type", [
  lva_service.GetOperatorRequest,
  dict,
])
def test_get_operator(request_type, transport: str = 'grpc'):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # Everything is optional in proto3 as far as the runtime is concerned,
    # and we are mocking out the actual API, so just send an empty request.
    request = request_type()

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.get_operator),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = lva_resources.Operator(
            name='name_value',
            docker_image='docker_image_value',
        )
        response = client.get_operator(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        assert args[0] == lva_service.GetOperatorRequest()

    # Establish that the response is the type that we expect.
    assert isinstance(response, lva_resources.Operator)
    assert response.name == 'name_value'
    assert response.docker_image == 'docker_image_value'


def test_get_operator_empty_call():
    # This test is a coverage failsafe to make sure that totally empty calls,
    # i.e. request == None and no flattened fields passed, work.
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport='grpc',
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.get_operator),
            '__call__') as call:
        client.get_operator()
        call.assert_called()
        _, args, _ = call.mock_calls[0]
        assert args[0] == lva_service.GetOperatorRequest()

@pytest.mark.asyncio
async def test_get_operator_async(transport: str = 'grpc_asyncio', request_type=lva_service.GetOperatorRequest):
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # Everything is optional in proto3 as far as the runtime is concerned,
    # and we are mocking out the actual API, so just send an empty request.
    request = request_type()

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.get_operator),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value =grpc_helpers_async.FakeUnaryUnaryCall(lva_resources.Operator(
            name='name_value',
            docker_image='docker_image_value',
        ))
        response = await client.get_operator(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls)
        _, args, _ = call.mock_calls[0]
        assert args[0] == lva_service.GetOperatorRequest()

    # Establish that the response is the type that we expect.
    assert isinstance(response, lva_resources.Operator)
    assert response.name == 'name_value'
    assert response.docker_image == 'docker_image_value'


@pytest.mark.asyncio
async def test_get_operator_async_from_dict():
    await test_get_operator_async(request_type=dict)


def test_get_operator_field_headers():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Any value that is part of the HTTP/1.1 URI should be sent as
    # a field header. Set these to a non-empty value.
    request = lva_service.GetOperatorRequest()

    request.name = 'name_value'

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.get_operator),
            '__call__') as call:
        call.return_value = lva_resources.Operator()
        client.get_operator(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        assert args[0] == request

    # Establish that the field header was sent.
    _, _, kw = call.mock_calls[0]
    assert (
        'x-goog-request-params',
        'name=name_value',
    ) in kw['metadata']


@pytest.mark.asyncio
async def test_get_operator_field_headers_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Any value that is part of the HTTP/1.1 URI should be sent as
    # a field header. Set these to a non-empty value.
    request = lva_service.GetOperatorRequest()

    request.name = 'name_value'

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.get_operator),
            '__call__') as call:
        call.return_value = grpc_helpers_async.FakeUnaryUnaryCall(lva_resources.Operator())
        await client.get_operator(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls)
        _, args, _ = call.mock_calls[0]
        assert args[0] == request

    # Establish that the field header was sent.
    _, _, kw = call.mock_calls[0]
    assert (
        'x-goog-request-params',
        'name=name_value',
    ) in kw['metadata']


def test_get_operator_flattened():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.get_operator),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = lva_resources.Operator()
        # Call the method with a truthy value for each flattened field,
        # using the keyword arguments to the method.
        client.get_operator(
            name='name_value',
        )

        # Establish that the underlying call was made with the expected
        # request object values.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        arg = args[0].name
        mock_val = 'name_value'
        assert arg == mock_val


def test_get_operator_flattened_error():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Attempting to call a method with both a request object and flattened
    # fields is an error.
    with pytest.raises(ValueError):
        client.get_operator(
            lva_service.GetOperatorRequest(),
            name='name_value',
        )

@pytest.mark.asyncio
async def test_get_operator_flattened_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.get_operator),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = lva_resources.Operator()

        call.return_value = grpc_helpers_async.FakeUnaryUnaryCall(lva_resources.Operator())
        # Call the method with a truthy value for each flattened field,
        # using the keyword arguments to the method.
        response = await client.get_operator(
            name='name_value',
        )

        # Establish that the underlying call was made with the expected
        # request object values.
        assert len(call.mock_calls)
        _, args, _ = call.mock_calls[0]
        arg = args[0].name
        mock_val = 'name_value'
        assert arg == mock_val

@pytest.mark.asyncio
async def test_get_operator_flattened_error_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Attempting to call a method with both a request object and flattened
    # fields is an error.
    with pytest.raises(ValueError):
        await client.get_operator(
            lva_service.GetOperatorRequest(),
            name='name_value',
        )


@pytest.mark.parametrize("request_type", [
  lva_service.CreateOperatorRequest,
  dict,
])
def test_create_operator(request_type, transport: str = 'grpc'):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # Everything is optional in proto3 as far as the runtime is concerned,
    # and we are mocking out the actual API, so just send an empty request.
    request = request_type()

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.create_operator),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = operations_pb2.Operation(name='operations/spam')
        response = client.create_operator(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        assert args[0] == lva_service.CreateOperatorRequest()

    # Establish that the response is the type that we expect.
    assert isinstance(response, future.Future)


def test_create_operator_empty_call():
    # This test is a coverage failsafe to make sure that totally empty calls,
    # i.e. request == None and no flattened fields passed, work.
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport='grpc',
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.create_operator),
            '__call__') as call:
        client.create_operator()
        call.assert_called()
        _, args, _ = call.mock_calls[0]
        assert args[0] == lva_service.CreateOperatorRequest()

@pytest.mark.asyncio
async def test_create_operator_async(transport: str = 'grpc_asyncio', request_type=lva_service.CreateOperatorRequest):
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # Everything is optional in proto3 as far as the runtime is concerned,
    # and we are mocking out the actual API, so just send an empty request.
    request = request_type()

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.create_operator),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = grpc_helpers_async.FakeUnaryUnaryCall(
            operations_pb2.Operation(name='operations/spam')
        )
        response = await client.create_operator(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls)
        _, args, _ = call.mock_calls[0]
        assert args[0] == lva_service.CreateOperatorRequest()

    # Establish that the response is the type that we expect.
    assert isinstance(response, future.Future)


@pytest.mark.asyncio
async def test_create_operator_async_from_dict():
    await test_create_operator_async(request_type=dict)


def test_create_operator_field_headers():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Any value that is part of the HTTP/1.1 URI should be sent as
    # a field header. Set these to a non-empty value.
    request = lva_service.CreateOperatorRequest()

    request.parent = 'parent_value'

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.create_operator),
            '__call__') as call:
        call.return_value = operations_pb2.Operation(name='operations/op')
        client.create_operator(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        assert args[0] == request

    # Establish that the field header was sent.
    _, _, kw = call.mock_calls[0]
    assert (
        'x-goog-request-params',
        'parent=parent_value',
    ) in kw['metadata']


@pytest.mark.asyncio
async def test_create_operator_field_headers_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Any value that is part of the HTTP/1.1 URI should be sent as
    # a field header. Set these to a non-empty value.
    request = lva_service.CreateOperatorRequest()

    request.parent = 'parent_value'

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.create_operator),
            '__call__') as call:
        call.return_value = grpc_helpers_async.FakeUnaryUnaryCall(operations_pb2.Operation(name='operations/op'))
        await client.create_operator(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls)
        _, args, _ = call.mock_calls[0]
        assert args[0] == request

    # Establish that the field header was sent.
    _, _, kw = call.mock_calls[0]
    assert (
        'x-goog-request-params',
        'parent=parent_value',
    ) in kw['metadata']


def test_create_operator_flattened():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.create_operator),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = operations_pb2.Operation(name='operations/op')
        # Call the method with a truthy value for each flattened field,
        # using the keyword arguments to the method.
        client.create_operator(
            parent='parent_value',
            operator=lva_resources.Operator(name='name_value'),
            operator_id='operator_id_value',
        )

        # Establish that the underlying call was made with the expected
        # request object values.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        arg = args[0].parent
        mock_val = 'parent_value'
        assert arg == mock_val
        arg = args[0].operator
        mock_val = lva_resources.Operator(name='name_value')
        assert arg == mock_val
        arg = args[0].operator_id
        mock_val = 'operator_id_value'
        assert arg == mock_val


def test_create_operator_flattened_error():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Attempting to call a method with both a request object and flattened
    # fields is an error.
    with pytest.raises(ValueError):
        client.create_operator(
            lva_service.CreateOperatorRequest(),
            parent='parent_value',
            operator=lva_resources.Operator(name='name_value'),
            operator_id='operator_id_value',
        )

@pytest.mark.asyncio
async def test_create_operator_flattened_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.create_operator),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = operations_pb2.Operation(name='operations/op')

        call.return_value = grpc_helpers_async.FakeUnaryUnaryCall(
            operations_pb2.Operation(name='operations/spam')
        )
        # Call the method with a truthy value for each flattened field,
        # using the keyword arguments to the method.
        response = await client.create_operator(
            parent='parent_value',
            operator=lva_resources.Operator(name='name_value'),
            operator_id='operator_id_value',
        )

        # Establish that the underlying call was made with the expected
        # request object values.
        assert len(call.mock_calls)
        _, args, _ = call.mock_calls[0]
        arg = args[0].parent
        mock_val = 'parent_value'
        assert arg == mock_val
        arg = args[0].operator
        mock_val = lva_resources.Operator(name='name_value')
        assert arg == mock_val
        arg = args[0].operator_id
        mock_val = 'operator_id_value'
        assert arg == mock_val

@pytest.mark.asyncio
async def test_create_operator_flattened_error_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Attempting to call a method with both a request object and flattened
    # fields is an error.
    with pytest.raises(ValueError):
        await client.create_operator(
            lva_service.CreateOperatorRequest(),
            parent='parent_value',
            operator=lva_resources.Operator(name='name_value'),
            operator_id='operator_id_value',
        )


@pytest.mark.parametrize("request_type", [
  lva_service.UpdateOperatorRequest,
  dict,
])
def test_update_operator(request_type, transport: str = 'grpc'):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # Everything is optional in proto3 as far as the runtime is concerned,
    # and we are mocking out the actual API, so just send an empty request.
    request = request_type()

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.update_operator),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = operations_pb2.Operation(name='operations/spam')
        response = client.update_operator(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        assert args[0] == lva_service.UpdateOperatorRequest()

    # Establish that the response is the type that we expect.
    assert isinstance(response, future.Future)


def test_update_operator_empty_call():
    # This test is a coverage failsafe to make sure that totally empty calls,
    # i.e. request == None and no flattened fields passed, work.
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport='grpc',
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.update_operator),
            '__call__') as call:
        client.update_operator()
        call.assert_called()
        _, args, _ = call.mock_calls[0]
        assert args[0] == lva_service.UpdateOperatorRequest()

@pytest.mark.asyncio
async def test_update_operator_async(transport: str = 'grpc_asyncio', request_type=lva_service.UpdateOperatorRequest):
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # Everything is optional in proto3 as far as the runtime is concerned,
    # and we are mocking out the actual API, so just send an empty request.
    request = request_type()

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.update_operator),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = grpc_helpers_async.FakeUnaryUnaryCall(
            operations_pb2.Operation(name='operations/spam')
        )
        response = await client.update_operator(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls)
        _, args, _ = call.mock_calls[0]
        assert args[0] == lva_service.UpdateOperatorRequest()

    # Establish that the response is the type that we expect.
    assert isinstance(response, future.Future)


@pytest.mark.asyncio
async def test_update_operator_async_from_dict():
    await test_update_operator_async(request_type=dict)


def test_update_operator_field_headers():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Any value that is part of the HTTP/1.1 URI should be sent as
    # a field header. Set these to a non-empty value.
    request = lva_service.UpdateOperatorRequest()

    request.operator.name = 'name_value'

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.update_operator),
            '__call__') as call:
        call.return_value = operations_pb2.Operation(name='operations/op')
        client.update_operator(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        assert args[0] == request

    # Establish that the field header was sent.
    _, _, kw = call.mock_calls[0]
    assert (
        'x-goog-request-params',
        'operator.name=name_value',
    ) in kw['metadata']


@pytest.mark.asyncio
async def test_update_operator_field_headers_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Any value that is part of the HTTP/1.1 URI should be sent as
    # a field header. Set these to a non-empty value.
    request = lva_service.UpdateOperatorRequest()

    request.operator.name = 'name_value'

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.update_operator),
            '__call__') as call:
        call.return_value = grpc_helpers_async.FakeUnaryUnaryCall(operations_pb2.Operation(name='operations/op'))
        await client.update_operator(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls)
        _, args, _ = call.mock_calls[0]
        assert args[0] == request

    # Establish that the field header was sent.
    _, _, kw = call.mock_calls[0]
    assert (
        'x-goog-request-params',
        'operator.name=name_value',
    ) in kw['metadata']


def test_update_operator_flattened():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.update_operator),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = operations_pb2.Operation(name='operations/op')
        # Call the method with a truthy value for each flattened field,
        # using the keyword arguments to the method.
        client.update_operator(
            operator=lva_resources.Operator(name='name_value'),
            update_mask=field_mask_pb2.FieldMask(paths=['paths_value']),
        )

        # Establish that the underlying call was made with the expected
        # request object values.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        arg = args[0].operator
        mock_val = lva_resources.Operator(name='name_value')
        assert arg == mock_val
        arg = args[0].update_mask
        mock_val = field_mask_pb2.FieldMask(paths=['paths_value'])
        assert arg == mock_val


def test_update_operator_flattened_error():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Attempting to call a method with both a request object and flattened
    # fields is an error.
    with pytest.raises(ValueError):
        client.update_operator(
            lva_service.UpdateOperatorRequest(),
            operator=lva_resources.Operator(name='name_value'),
            update_mask=field_mask_pb2.FieldMask(paths=['paths_value']),
        )

@pytest.mark.asyncio
async def test_update_operator_flattened_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.update_operator),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = operations_pb2.Operation(name='operations/op')

        call.return_value = grpc_helpers_async.FakeUnaryUnaryCall(
            operations_pb2.Operation(name='operations/spam')
        )
        # Call the method with a truthy value for each flattened field,
        # using the keyword arguments to the method.
        response = await client.update_operator(
            operator=lva_resources.Operator(name='name_value'),
            update_mask=field_mask_pb2.FieldMask(paths=['paths_value']),
        )

        # Establish that the underlying call was made with the expected
        # request object values.
        assert len(call.mock_calls)
        _, args, _ = call.mock_calls[0]
        arg = args[0].operator
        mock_val = lva_resources.Operator(name='name_value')
        assert arg == mock_val
        arg = args[0].update_mask
        mock_val = field_mask_pb2.FieldMask(paths=['paths_value'])
        assert arg == mock_val

@pytest.mark.asyncio
async def test_update_operator_flattened_error_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Attempting to call a method with both a request object and flattened
    # fields is an error.
    with pytest.raises(ValueError):
        await client.update_operator(
            lva_service.UpdateOperatorRequest(),
            operator=lva_resources.Operator(name='name_value'),
            update_mask=field_mask_pb2.FieldMask(paths=['paths_value']),
        )


@pytest.mark.parametrize("request_type", [
  lva_service.DeleteOperatorRequest,
  dict,
])
def test_delete_operator(request_type, transport: str = 'grpc'):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # Everything is optional in proto3 as far as the runtime is concerned,
    # and we are mocking out the actual API, so just send an empty request.
    request = request_type()

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.delete_operator),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = operations_pb2.Operation(name='operations/spam')
        response = client.delete_operator(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        assert args[0] == lva_service.DeleteOperatorRequest()

    # Establish that the response is the type that we expect.
    assert isinstance(response, future.Future)


def test_delete_operator_empty_call():
    # This test is a coverage failsafe to make sure that totally empty calls,
    # i.e. request == None and no flattened fields passed, work.
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport='grpc',
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.delete_operator),
            '__call__') as call:
        client.delete_operator()
        call.assert_called()
        _, args, _ = call.mock_calls[0]
        assert args[0] == lva_service.DeleteOperatorRequest()

@pytest.mark.asyncio
async def test_delete_operator_async(transport: str = 'grpc_asyncio', request_type=lva_service.DeleteOperatorRequest):
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # Everything is optional in proto3 as far as the runtime is concerned,
    # and we are mocking out the actual API, so just send an empty request.
    request = request_type()

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.delete_operator),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = grpc_helpers_async.FakeUnaryUnaryCall(
            operations_pb2.Operation(name='operations/spam')
        )
        response = await client.delete_operator(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls)
        _, args, _ = call.mock_calls[0]
        assert args[0] == lva_service.DeleteOperatorRequest()

    # Establish that the response is the type that we expect.
    assert isinstance(response, future.Future)


@pytest.mark.asyncio
async def test_delete_operator_async_from_dict():
    await test_delete_operator_async(request_type=dict)


def test_delete_operator_field_headers():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Any value that is part of the HTTP/1.1 URI should be sent as
    # a field header. Set these to a non-empty value.
    request = lva_service.DeleteOperatorRequest()

    request.name = 'name_value'

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.delete_operator),
            '__call__') as call:
        call.return_value = operations_pb2.Operation(name='operations/op')
        client.delete_operator(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        assert args[0] == request

    # Establish that the field header was sent.
    _, _, kw = call.mock_calls[0]
    assert (
        'x-goog-request-params',
        'name=name_value',
    ) in kw['metadata']


@pytest.mark.asyncio
async def test_delete_operator_field_headers_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Any value that is part of the HTTP/1.1 URI should be sent as
    # a field header. Set these to a non-empty value.
    request = lva_service.DeleteOperatorRequest()

    request.name = 'name_value'

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.delete_operator),
            '__call__') as call:
        call.return_value = grpc_helpers_async.FakeUnaryUnaryCall(operations_pb2.Operation(name='operations/op'))
        await client.delete_operator(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls)
        _, args, _ = call.mock_calls[0]
        assert args[0] == request

    # Establish that the field header was sent.
    _, _, kw = call.mock_calls[0]
    assert (
        'x-goog-request-params',
        'name=name_value',
    ) in kw['metadata']


def test_delete_operator_flattened():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.delete_operator),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = operations_pb2.Operation(name='operations/op')
        # Call the method with a truthy value for each flattened field,
        # using the keyword arguments to the method.
        client.delete_operator(
            name='name_value',
        )

        # Establish that the underlying call was made with the expected
        # request object values.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        arg = args[0].name
        mock_val = 'name_value'
        assert arg == mock_val


def test_delete_operator_flattened_error():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Attempting to call a method with both a request object and flattened
    # fields is an error.
    with pytest.raises(ValueError):
        client.delete_operator(
            lva_service.DeleteOperatorRequest(),
            name='name_value',
        )

@pytest.mark.asyncio
async def test_delete_operator_flattened_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.delete_operator),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = operations_pb2.Operation(name='operations/op')

        call.return_value = grpc_helpers_async.FakeUnaryUnaryCall(
            operations_pb2.Operation(name='operations/spam')
        )
        # Call the method with a truthy value for each flattened field,
        # using the keyword arguments to the method.
        response = await client.delete_operator(
            name='name_value',
        )

        # Establish that the underlying call was made with the expected
        # request object values.
        assert len(call.mock_calls)
        _, args, _ = call.mock_calls[0]
        arg = args[0].name
        mock_val = 'name_value'
        assert arg == mock_val

@pytest.mark.asyncio
async def test_delete_operator_flattened_error_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Attempting to call a method with both a request object and flattened
    # fields is an error.
    with pytest.raises(ValueError):
        await client.delete_operator(
            lva_service.DeleteOperatorRequest(),
            name='name_value',
        )


@pytest.mark.parametrize("request_type", [
  lva_service.ListAnalysesRequest,
  dict,
])
def test_list_analyses(request_type, transport: str = 'grpc'):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # Everything is optional in proto3 as far as the runtime is concerned,
    # and we are mocking out the actual API, so just send an empty request.
    request = request_type()

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.list_analyses),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = lva_service.ListAnalysesResponse(
            next_page_token='next_page_token_value',
            unreachable=['unreachable_value'],
        )
        response = client.list_analyses(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        assert args[0] == lva_service.ListAnalysesRequest()

    # Establish that the response is the type that we expect.
    assert isinstance(response, pagers.ListAnalysesPager)
    assert response.next_page_token == 'next_page_token_value'
    assert response.unreachable == ['unreachable_value']


def test_list_analyses_empty_call():
    # This test is a coverage failsafe to make sure that totally empty calls,
    # i.e. request == None and no flattened fields passed, work.
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport='grpc',
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.list_analyses),
            '__call__') as call:
        client.list_analyses()
        call.assert_called()
        _, args, _ = call.mock_calls[0]
        assert args[0] == lva_service.ListAnalysesRequest()

@pytest.mark.asyncio
async def test_list_analyses_async(transport: str = 'grpc_asyncio', request_type=lva_service.ListAnalysesRequest):
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # Everything is optional in proto3 as far as the runtime is concerned,
    # and we are mocking out the actual API, so just send an empty request.
    request = request_type()

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.list_analyses),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value =grpc_helpers_async.FakeUnaryUnaryCall(lva_service.ListAnalysesResponse(
            next_page_token='next_page_token_value',
            unreachable=['unreachable_value'],
        ))
        response = await client.list_analyses(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls)
        _, args, _ = call.mock_calls[0]
        assert args[0] == lva_service.ListAnalysesRequest()

    # Establish that the response is the type that we expect.
    assert isinstance(response, pagers.ListAnalysesAsyncPager)
    assert response.next_page_token == 'next_page_token_value'
    assert response.unreachable == ['unreachable_value']


@pytest.mark.asyncio
async def test_list_analyses_async_from_dict():
    await test_list_analyses_async(request_type=dict)


def test_list_analyses_field_headers():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Any value that is part of the HTTP/1.1 URI should be sent as
    # a field header. Set these to a non-empty value.
    request = lva_service.ListAnalysesRequest()

    request.parent = 'parent_value'

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.list_analyses),
            '__call__') as call:
        call.return_value = lva_service.ListAnalysesResponse()
        client.list_analyses(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        assert args[0] == request

    # Establish that the field header was sent.
    _, _, kw = call.mock_calls[0]
    assert (
        'x-goog-request-params',
        'parent=parent_value',
    ) in kw['metadata']


@pytest.mark.asyncio
async def test_list_analyses_field_headers_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Any value that is part of the HTTP/1.1 URI should be sent as
    # a field header. Set these to a non-empty value.
    request = lva_service.ListAnalysesRequest()

    request.parent = 'parent_value'

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.list_analyses),
            '__call__') as call:
        call.return_value = grpc_helpers_async.FakeUnaryUnaryCall(lva_service.ListAnalysesResponse())
        await client.list_analyses(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls)
        _, args, _ = call.mock_calls[0]
        assert args[0] == request

    # Establish that the field header was sent.
    _, _, kw = call.mock_calls[0]
    assert (
        'x-goog-request-params',
        'parent=parent_value',
    ) in kw['metadata']


def test_list_analyses_flattened():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.list_analyses),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = lva_service.ListAnalysesResponse()
        # Call the method with a truthy value for each flattened field,
        # using the keyword arguments to the method.
        client.list_analyses(
            parent='parent_value',
        )

        # Establish that the underlying call was made with the expected
        # request object values.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        arg = args[0].parent
        mock_val = 'parent_value'
        assert arg == mock_val


def test_list_analyses_flattened_error():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Attempting to call a method with both a request object and flattened
    # fields is an error.
    with pytest.raises(ValueError):
        client.list_analyses(
            lva_service.ListAnalysesRequest(),
            parent='parent_value',
        )

@pytest.mark.asyncio
async def test_list_analyses_flattened_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.list_analyses),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = lva_service.ListAnalysesResponse()

        call.return_value = grpc_helpers_async.FakeUnaryUnaryCall(lva_service.ListAnalysesResponse())
        # Call the method with a truthy value for each flattened field,
        # using the keyword arguments to the method.
        response = await client.list_analyses(
            parent='parent_value',
        )

        # Establish that the underlying call was made with the expected
        # request object values.
        assert len(call.mock_calls)
        _, args, _ = call.mock_calls[0]
        arg = args[0].parent
        mock_val = 'parent_value'
        assert arg == mock_val

@pytest.mark.asyncio
async def test_list_analyses_flattened_error_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Attempting to call a method with both a request object and flattened
    # fields is an error.
    with pytest.raises(ValueError):
        await client.list_analyses(
            lva_service.ListAnalysesRequest(),
            parent='parent_value',
        )


def test_list_analyses_pager(transport_name: str = "grpc"):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials,
        transport=transport_name,
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.list_analyses),
            '__call__') as call:
        # Set the response to a series of pages.
        call.side_effect = (
            lva_service.ListAnalysesResponse(
                analyses=[
                    lva_resources.Analysis(),
                    lva_resources.Analysis(),
                    lva_resources.Analysis(),
                ],
                next_page_token='abc',
            ),
            lva_service.ListAnalysesResponse(
                analyses=[],
                next_page_token='def',
            ),
            lva_service.ListAnalysesResponse(
                analyses=[
                    lva_resources.Analysis(),
                ],
                next_page_token='ghi',
            ),
            lva_service.ListAnalysesResponse(
                analyses=[
                    lva_resources.Analysis(),
                    lva_resources.Analysis(),
                ],
            ),
            RuntimeError,
        )

        metadata = ()
        metadata = tuple(metadata) + (
            gapic_v1.routing_header.to_grpc_metadata((
                ('parent', ''),
            )),
        )
        pager = client.list_analyses(request={})

        assert pager._metadata == metadata

        results = list(pager)
        assert len(results) == 6
        assert all(isinstance(i, lva_resources.Analysis)
                   for i in results)
def test_list_analyses_pages(transport_name: str = "grpc"):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials,
        transport=transport_name,
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.list_analyses),
            '__call__') as call:
        # Set the response to a series of pages.
        call.side_effect = (
            lva_service.ListAnalysesResponse(
                analyses=[
                    lva_resources.Analysis(),
                    lva_resources.Analysis(),
                    lva_resources.Analysis(),
                ],
                next_page_token='abc',
            ),
            lva_service.ListAnalysesResponse(
                analyses=[],
                next_page_token='def',
            ),
            lva_service.ListAnalysesResponse(
                analyses=[
                    lva_resources.Analysis(),
                ],
                next_page_token='ghi',
            ),
            lva_service.ListAnalysesResponse(
                analyses=[
                    lva_resources.Analysis(),
                    lva_resources.Analysis(),
                ],
            ),
            RuntimeError,
        )
        pages = list(client.list_analyses(request={}).pages)
        for page_, token in zip(pages, ['abc','def','ghi', '']):
            assert page_.raw_page.next_page_token == token

@pytest.mark.asyncio
async def test_list_analyses_async_pager():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials,
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.list_analyses),
            '__call__', new_callable=mock.AsyncMock) as call:
        # Set the response to a series of pages.
        call.side_effect = (
            lva_service.ListAnalysesResponse(
                analyses=[
                    lva_resources.Analysis(),
                    lva_resources.Analysis(),
                    lva_resources.Analysis(),
                ],
                next_page_token='abc',
            ),
            lva_service.ListAnalysesResponse(
                analyses=[],
                next_page_token='def',
            ),
            lva_service.ListAnalysesResponse(
                analyses=[
                    lva_resources.Analysis(),
                ],
                next_page_token='ghi',
            ),
            lva_service.ListAnalysesResponse(
                analyses=[
                    lva_resources.Analysis(),
                    lva_resources.Analysis(),
                ],
            ),
            RuntimeError,
        )
        async_pager = await client.list_analyses(request={},)
        assert async_pager.next_page_token == 'abc'
        responses = []
        async for response in async_pager: # pragma: no branch
            responses.append(response)

        assert len(responses) == 6
        assert all(isinstance(i, lva_resources.Analysis)
                for i in responses)


@pytest.mark.asyncio
async def test_list_analyses_async_pages():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials,
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.list_analyses),
            '__call__', new_callable=mock.AsyncMock) as call:
        # Set the response to a series of pages.
        call.side_effect = (
            lva_service.ListAnalysesResponse(
                analyses=[
                    lva_resources.Analysis(),
                    lva_resources.Analysis(),
                    lva_resources.Analysis(),
                ],
                next_page_token='abc',
            ),
            lva_service.ListAnalysesResponse(
                analyses=[],
                next_page_token='def',
            ),
            lva_service.ListAnalysesResponse(
                analyses=[
                    lva_resources.Analysis(),
                ],
                next_page_token='ghi',
            ),
            lva_service.ListAnalysesResponse(
                analyses=[
                    lva_resources.Analysis(),
                    lva_resources.Analysis(),
                ],
            ),
            RuntimeError,
        )
        pages = []
        # Workaround issue in python 3.9 related to code coverage by adding `# pragma: no branch`
        # See https://github.com/googleapis/gapic-generator-python/pull/1174#issuecomment-1025132372
        async for page_ in ( # pragma: no branch
            await client.list_analyses(request={})
        ).pages:
            pages.append(page_)
        for page_, token in zip(pages, ['abc','def','ghi', '']):
            assert page_.raw_page.next_page_token == token

@pytest.mark.parametrize("request_type", [
  lva_service.GetAnalysisRequest,
  dict,
])
def test_get_analysis(request_type, transport: str = 'grpc'):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # Everything is optional in proto3 as far as the runtime is concerned,
    # and we are mocking out the actual API, so just send an empty request.
    request = request_type()

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.get_analysis),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = lva_resources.Analysis(
            name='name_value',
            disable_event_watch=True,
        )
        response = client.get_analysis(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        assert args[0] == lva_service.GetAnalysisRequest()

    # Establish that the response is the type that we expect.
    assert isinstance(response, lva_resources.Analysis)
    assert response.name == 'name_value'
    assert response.disable_event_watch is True


def test_get_analysis_empty_call():
    # This test is a coverage failsafe to make sure that totally empty calls,
    # i.e. request == None and no flattened fields passed, work.
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport='grpc',
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.get_analysis),
            '__call__') as call:
        client.get_analysis()
        call.assert_called()
        _, args, _ = call.mock_calls[0]
        assert args[0] == lva_service.GetAnalysisRequest()

@pytest.mark.asyncio
async def test_get_analysis_async(transport: str = 'grpc_asyncio', request_type=lva_service.GetAnalysisRequest):
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # Everything is optional in proto3 as far as the runtime is concerned,
    # and we are mocking out the actual API, so just send an empty request.
    request = request_type()

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.get_analysis),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value =grpc_helpers_async.FakeUnaryUnaryCall(lva_resources.Analysis(
            name='name_value',
            disable_event_watch=True,
        ))
        response = await client.get_analysis(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls)
        _, args, _ = call.mock_calls[0]
        assert args[0] == lva_service.GetAnalysisRequest()

    # Establish that the response is the type that we expect.
    assert isinstance(response, lva_resources.Analysis)
    assert response.name == 'name_value'
    assert response.disable_event_watch is True


@pytest.mark.asyncio
async def test_get_analysis_async_from_dict():
    await test_get_analysis_async(request_type=dict)


def test_get_analysis_field_headers():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Any value that is part of the HTTP/1.1 URI should be sent as
    # a field header. Set these to a non-empty value.
    request = lva_service.GetAnalysisRequest()

    request.name = 'name_value'

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.get_analysis),
            '__call__') as call:
        call.return_value = lva_resources.Analysis()
        client.get_analysis(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        assert args[0] == request

    # Establish that the field header was sent.
    _, _, kw = call.mock_calls[0]
    assert (
        'x-goog-request-params',
        'name=name_value',
    ) in kw['metadata']


@pytest.mark.asyncio
async def test_get_analysis_field_headers_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Any value that is part of the HTTP/1.1 URI should be sent as
    # a field header. Set these to a non-empty value.
    request = lva_service.GetAnalysisRequest()

    request.name = 'name_value'

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.get_analysis),
            '__call__') as call:
        call.return_value = grpc_helpers_async.FakeUnaryUnaryCall(lva_resources.Analysis())
        await client.get_analysis(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls)
        _, args, _ = call.mock_calls[0]
        assert args[0] == request

    # Establish that the field header was sent.
    _, _, kw = call.mock_calls[0]
    assert (
        'x-goog-request-params',
        'name=name_value',
    ) in kw['metadata']


def test_get_analysis_flattened():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.get_analysis),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = lva_resources.Analysis()
        # Call the method with a truthy value for each flattened field,
        # using the keyword arguments to the method.
        client.get_analysis(
            name='name_value',
        )

        # Establish that the underlying call was made with the expected
        # request object values.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        arg = args[0].name
        mock_val = 'name_value'
        assert arg == mock_val


def test_get_analysis_flattened_error():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Attempting to call a method with both a request object and flattened
    # fields is an error.
    with pytest.raises(ValueError):
        client.get_analysis(
            lva_service.GetAnalysisRequest(),
            name='name_value',
        )

@pytest.mark.asyncio
async def test_get_analysis_flattened_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.get_analysis),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = lva_resources.Analysis()

        call.return_value = grpc_helpers_async.FakeUnaryUnaryCall(lva_resources.Analysis())
        # Call the method with a truthy value for each flattened field,
        # using the keyword arguments to the method.
        response = await client.get_analysis(
            name='name_value',
        )

        # Establish that the underlying call was made with the expected
        # request object values.
        assert len(call.mock_calls)
        _, args, _ = call.mock_calls[0]
        arg = args[0].name
        mock_val = 'name_value'
        assert arg == mock_val

@pytest.mark.asyncio
async def test_get_analysis_flattened_error_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Attempting to call a method with both a request object and flattened
    # fields is an error.
    with pytest.raises(ValueError):
        await client.get_analysis(
            lva_service.GetAnalysisRequest(),
            name='name_value',
        )


@pytest.mark.parametrize("request_type", [
  lva_service.CreateAnalysisRequest,
  dict,
])
def test_create_analysis(request_type, transport: str = 'grpc'):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # Everything is optional in proto3 as far as the runtime is concerned,
    # and we are mocking out the actual API, so just send an empty request.
    request = request_type()

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.create_analysis),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = operations_pb2.Operation(name='operations/spam')
        response = client.create_analysis(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        assert args[0] == lva_service.CreateAnalysisRequest()

    # Establish that the response is the type that we expect.
    assert isinstance(response, future.Future)


def test_create_analysis_empty_call():
    # This test is a coverage failsafe to make sure that totally empty calls,
    # i.e. request == None and no flattened fields passed, work.
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport='grpc',
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.create_analysis),
            '__call__') as call:
        client.create_analysis()
        call.assert_called()
        _, args, _ = call.mock_calls[0]
        assert args[0] == lva_service.CreateAnalysisRequest()

@pytest.mark.asyncio
async def test_create_analysis_async(transport: str = 'grpc_asyncio', request_type=lva_service.CreateAnalysisRequest):
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # Everything is optional in proto3 as far as the runtime is concerned,
    # and we are mocking out the actual API, so just send an empty request.
    request = request_type()

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.create_analysis),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = grpc_helpers_async.FakeUnaryUnaryCall(
            operations_pb2.Operation(name='operations/spam')
        )
        response = await client.create_analysis(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls)
        _, args, _ = call.mock_calls[0]
        assert args[0] == lva_service.CreateAnalysisRequest()

    # Establish that the response is the type that we expect.
    assert isinstance(response, future.Future)


@pytest.mark.asyncio
async def test_create_analysis_async_from_dict():
    await test_create_analysis_async(request_type=dict)


def test_create_analysis_field_headers():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Any value that is part of the HTTP/1.1 URI should be sent as
    # a field header. Set these to a non-empty value.
    request = lva_service.CreateAnalysisRequest()

    request.parent = 'parent_value'

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.create_analysis),
            '__call__') as call:
        call.return_value = operations_pb2.Operation(name='operations/op')
        client.create_analysis(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        assert args[0] == request

    # Establish that the field header was sent.
    _, _, kw = call.mock_calls[0]
    assert (
        'x-goog-request-params',
        'parent=parent_value',
    ) in kw['metadata']


@pytest.mark.asyncio
async def test_create_analysis_field_headers_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Any value that is part of the HTTP/1.1 URI should be sent as
    # a field header. Set these to a non-empty value.
    request = lva_service.CreateAnalysisRequest()

    request.parent = 'parent_value'

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.create_analysis),
            '__call__') as call:
        call.return_value = grpc_helpers_async.FakeUnaryUnaryCall(operations_pb2.Operation(name='operations/op'))
        await client.create_analysis(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls)
        _, args, _ = call.mock_calls[0]
        assert args[0] == request

    # Establish that the field header was sent.
    _, _, kw = call.mock_calls[0]
    assert (
        'x-goog-request-params',
        'parent=parent_value',
    ) in kw['metadata']


def test_create_analysis_flattened():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.create_analysis),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = operations_pb2.Operation(name='operations/op')
        # Call the method with a truthy value for each flattened field,
        # using the keyword arguments to the method.
        client.create_analysis(
            parent='parent_value',
            analysis=lva_resources.Analysis(name='name_value'),
            analysis_id='analysis_id_value',
        )

        # Establish that the underlying call was made with the expected
        # request object values.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        arg = args[0].parent
        mock_val = 'parent_value'
        assert arg == mock_val
        arg = args[0].analysis
        mock_val = lva_resources.Analysis(name='name_value')
        assert arg == mock_val
        arg = args[0].analysis_id
        mock_val = 'analysis_id_value'
        assert arg == mock_val


def test_create_analysis_flattened_error():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Attempting to call a method with both a request object and flattened
    # fields is an error.
    with pytest.raises(ValueError):
        client.create_analysis(
            lva_service.CreateAnalysisRequest(),
            parent='parent_value',
            analysis=lva_resources.Analysis(name='name_value'),
            analysis_id='analysis_id_value',
        )

@pytest.mark.asyncio
async def test_create_analysis_flattened_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.create_analysis),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = operations_pb2.Operation(name='operations/op')

        call.return_value = grpc_helpers_async.FakeUnaryUnaryCall(
            operations_pb2.Operation(name='operations/spam')
        )
        # Call the method with a truthy value for each flattened field,
        # using the keyword arguments to the method.
        response = await client.create_analysis(
            parent='parent_value',
            analysis=lva_resources.Analysis(name='name_value'),
            analysis_id='analysis_id_value',
        )

        # Establish that the underlying call was made with the expected
        # request object values.
        assert len(call.mock_calls)
        _, args, _ = call.mock_calls[0]
        arg = args[0].parent
        mock_val = 'parent_value'
        assert arg == mock_val
        arg = args[0].analysis
        mock_val = lva_resources.Analysis(name='name_value')
        assert arg == mock_val
        arg = args[0].analysis_id
        mock_val = 'analysis_id_value'
        assert arg == mock_val

@pytest.mark.asyncio
async def test_create_analysis_flattened_error_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Attempting to call a method with both a request object and flattened
    # fields is an error.
    with pytest.raises(ValueError):
        await client.create_analysis(
            lva_service.CreateAnalysisRequest(),
            parent='parent_value',
            analysis=lva_resources.Analysis(name='name_value'),
            analysis_id='analysis_id_value',
        )


@pytest.mark.parametrize("request_type", [
  lva_service.UpdateAnalysisRequest,
  dict,
])
def test_update_analysis(request_type, transport: str = 'grpc'):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # Everything is optional in proto3 as far as the runtime is concerned,
    # and we are mocking out the actual API, so just send an empty request.
    request = request_type()

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.update_analysis),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = operations_pb2.Operation(name='operations/spam')
        response = client.update_analysis(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        assert args[0] == lva_service.UpdateAnalysisRequest()

    # Establish that the response is the type that we expect.
    assert isinstance(response, future.Future)


def test_update_analysis_empty_call():
    # This test is a coverage failsafe to make sure that totally empty calls,
    # i.e. request == None and no flattened fields passed, work.
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport='grpc',
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.update_analysis),
            '__call__') as call:
        client.update_analysis()
        call.assert_called()
        _, args, _ = call.mock_calls[0]
        assert args[0] == lva_service.UpdateAnalysisRequest()

@pytest.mark.asyncio
async def test_update_analysis_async(transport: str = 'grpc_asyncio', request_type=lva_service.UpdateAnalysisRequest):
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # Everything is optional in proto3 as far as the runtime is concerned,
    # and we are mocking out the actual API, so just send an empty request.
    request = request_type()

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.update_analysis),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = grpc_helpers_async.FakeUnaryUnaryCall(
            operations_pb2.Operation(name='operations/spam')
        )
        response = await client.update_analysis(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls)
        _, args, _ = call.mock_calls[0]
        assert args[0] == lva_service.UpdateAnalysisRequest()

    # Establish that the response is the type that we expect.
    assert isinstance(response, future.Future)


@pytest.mark.asyncio
async def test_update_analysis_async_from_dict():
    await test_update_analysis_async(request_type=dict)


def test_update_analysis_field_headers():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Any value that is part of the HTTP/1.1 URI should be sent as
    # a field header. Set these to a non-empty value.
    request = lva_service.UpdateAnalysisRequest()

    request.analysis.name = 'name_value'

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.update_analysis),
            '__call__') as call:
        call.return_value = operations_pb2.Operation(name='operations/op')
        client.update_analysis(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        assert args[0] == request

    # Establish that the field header was sent.
    _, _, kw = call.mock_calls[0]
    assert (
        'x-goog-request-params',
        'analysis.name=name_value',
    ) in kw['metadata']


@pytest.mark.asyncio
async def test_update_analysis_field_headers_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Any value that is part of the HTTP/1.1 URI should be sent as
    # a field header. Set these to a non-empty value.
    request = lva_service.UpdateAnalysisRequest()

    request.analysis.name = 'name_value'

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.update_analysis),
            '__call__') as call:
        call.return_value = grpc_helpers_async.FakeUnaryUnaryCall(operations_pb2.Operation(name='operations/op'))
        await client.update_analysis(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls)
        _, args, _ = call.mock_calls[0]
        assert args[0] == request

    # Establish that the field header was sent.
    _, _, kw = call.mock_calls[0]
    assert (
        'x-goog-request-params',
        'analysis.name=name_value',
    ) in kw['metadata']


def test_update_analysis_flattened():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.update_analysis),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = operations_pb2.Operation(name='operations/op')
        # Call the method with a truthy value for each flattened field,
        # using the keyword arguments to the method.
        client.update_analysis(
            analysis=lva_resources.Analysis(name='name_value'),
            update_mask=field_mask_pb2.FieldMask(paths=['paths_value']),
        )

        # Establish that the underlying call was made with the expected
        # request object values.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        arg = args[0].analysis
        mock_val = lva_resources.Analysis(name='name_value')
        assert arg == mock_val
        arg = args[0].update_mask
        mock_val = field_mask_pb2.FieldMask(paths=['paths_value'])
        assert arg == mock_val


def test_update_analysis_flattened_error():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Attempting to call a method with both a request object and flattened
    # fields is an error.
    with pytest.raises(ValueError):
        client.update_analysis(
            lva_service.UpdateAnalysisRequest(),
            analysis=lva_resources.Analysis(name='name_value'),
            update_mask=field_mask_pb2.FieldMask(paths=['paths_value']),
        )

@pytest.mark.asyncio
async def test_update_analysis_flattened_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.update_analysis),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = operations_pb2.Operation(name='operations/op')

        call.return_value = grpc_helpers_async.FakeUnaryUnaryCall(
            operations_pb2.Operation(name='operations/spam')
        )
        # Call the method with a truthy value for each flattened field,
        # using the keyword arguments to the method.
        response = await client.update_analysis(
            analysis=lva_resources.Analysis(name='name_value'),
            update_mask=field_mask_pb2.FieldMask(paths=['paths_value']),
        )

        # Establish that the underlying call was made with the expected
        # request object values.
        assert len(call.mock_calls)
        _, args, _ = call.mock_calls[0]
        arg = args[0].analysis
        mock_val = lva_resources.Analysis(name='name_value')
        assert arg == mock_val
        arg = args[0].update_mask
        mock_val = field_mask_pb2.FieldMask(paths=['paths_value'])
        assert arg == mock_val

@pytest.mark.asyncio
async def test_update_analysis_flattened_error_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Attempting to call a method with both a request object and flattened
    # fields is an error.
    with pytest.raises(ValueError):
        await client.update_analysis(
            lva_service.UpdateAnalysisRequest(),
            analysis=lva_resources.Analysis(name='name_value'),
            update_mask=field_mask_pb2.FieldMask(paths=['paths_value']),
        )


@pytest.mark.parametrize("request_type", [
  lva_service.DeleteAnalysisRequest,
  dict,
])
def test_delete_analysis(request_type, transport: str = 'grpc'):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # Everything is optional in proto3 as far as the runtime is concerned,
    # and we are mocking out the actual API, so just send an empty request.
    request = request_type()

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.delete_analysis),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = operations_pb2.Operation(name='operations/spam')
        response = client.delete_analysis(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        assert args[0] == lva_service.DeleteAnalysisRequest()

    # Establish that the response is the type that we expect.
    assert isinstance(response, future.Future)


def test_delete_analysis_empty_call():
    # This test is a coverage failsafe to make sure that totally empty calls,
    # i.e. request == None and no flattened fields passed, work.
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport='grpc',
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.delete_analysis),
            '__call__') as call:
        client.delete_analysis()
        call.assert_called()
        _, args, _ = call.mock_calls[0]
        assert args[0] == lva_service.DeleteAnalysisRequest()

@pytest.mark.asyncio
async def test_delete_analysis_async(transport: str = 'grpc_asyncio', request_type=lva_service.DeleteAnalysisRequest):
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # Everything is optional in proto3 as far as the runtime is concerned,
    # and we are mocking out the actual API, so just send an empty request.
    request = request_type()

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.delete_analysis),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = grpc_helpers_async.FakeUnaryUnaryCall(
            operations_pb2.Operation(name='operations/spam')
        )
        response = await client.delete_analysis(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls)
        _, args, _ = call.mock_calls[0]
        assert args[0] == lva_service.DeleteAnalysisRequest()

    # Establish that the response is the type that we expect.
    assert isinstance(response, future.Future)


@pytest.mark.asyncio
async def test_delete_analysis_async_from_dict():
    await test_delete_analysis_async(request_type=dict)


def test_delete_analysis_field_headers():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Any value that is part of the HTTP/1.1 URI should be sent as
    # a field header. Set these to a non-empty value.
    request = lva_service.DeleteAnalysisRequest()

    request.name = 'name_value'

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.delete_analysis),
            '__call__') as call:
        call.return_value = operations_pb2.Operation(name='operations/op')
        client.delete_analysis(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        assert args[0] == request

    # Establish that the field header was sent.
    _, _, kw = call.mock_calls[0]
    assert (
        'x-goog-request-params',
        'name=name_value',
    ) in kw['metadata']


@pytest.mark.asyncio
async def test_delete_analysis_field_headers_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Any value that is part of the HTTP/1.1 URI should be sent as
    # a field header. Set these to a non-empty value.
    request = lva_service.DeleteAnalysisRequest()

    request.name = 'name_value'

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.delete_analysis),
            '__call__') as call:
        call.return_value = grpc_helpers_async.FakeUnaryUnaryCall(operations_pb2.Operation(name='operations/op'))
        await client.delete_analysis(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls)
        _, args, _ = call.mock_calls[0]
        assert args[0] == request

    # Establish that the field header was sent.
    _, _, kw = call.mock_calls[0]
    assert (
        'x-goog-request-params',
        'name=name_value',
    ) in kw['metadata']


def test_delete_analysis_flattened():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.delete_analysis),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = operations_pb2.Operation(name='operations/op')
        # Call the method with a truthy value for each flattened field,
        # using the keyword arguments to the method.
        client.delete_analysis(
            name='name_value',
        )

        # Establish that the underlying call was made with the expected
        # request object values.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        arg = args[0].name
        mock_val = 'name_value'
        assert arg == mock_val


def test_delete_analysis_flattened_error():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Attempting to call a method with both a request object and flattened
    # fields is an error.
    with pytest.raises(ValueError):
        client.delete_analysis(
            lva_service.DeleteAnalysisRequest(),
            name='name_value',
        )

@pytest.mark.asyncio
async def test_delete_analysis_flattened_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.delete_analysis),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = operations_pb2.Operation(name='operations/op')

        call.return_value = grpc_helpers_async.FakeUnaryUnaryCall(
            operations_pb2.Operation(name='operations/spam')
        )
        # Call the method with a truthy value for each flattened field,
        # using the keyword arguments to the method.
        response = await client.delete_analysis(
            name='name_value',
        )

        # Establish that the underlying call was made with the expected
        # request object values.
        assert len(call.mock_calls)
        _, args, _ = call.mock_calls[0]
        arg = args[0].name
        mock_val = 'name_value'
        assert arg == mock_val

@pytest.mark.asyncio
async def test_delete_analysis_flattened_error_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Attempting to call a method with both a request object and flattened
    # fields is an error.
    with pytest.raises(ValueError):
        await client.delete_analysis(
            lva_service.DeleteAnalysisRequest(),
            name='name_value',
        )


@pytest.mark.parametrize("request_type", [
  lva_service.ListProcessesRequest,
  dict,
])
def test_list_processes(request_type, transport: str = 'grpc'):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # Everything is optional in proto3 as far as the runtime is concerned,
    # and we are mocking out the actual API, so just send an empty request.
    request = request_type()

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.list_processes),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = lva_service.ListProcessesResponse(
            next_page_token='next_page_token_value',
            unreachable=['unreachable_value'],
        )
        response = client.list_processes(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        assert args[0] == lva_service.ListProcessesRequest()

    # Establish that the response is the type that we expect.
    assert isinstance(response, pagers.ListProcessesPager)
    assert response.next_page_token == 'next_page_token_value'
    assert response.unreachable == ['unreachable_value']


def test_list_processes_empty_call():
    # This test is a coverage failsafe to make sure that totally empty calls,
    # i.e. request == None and no flattened fields passed, work.
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport='grpc',
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.list_processes),
            '__call__') as call:
        client.list_processes()
        call.assert_called()
        _, args, _ = call.mock_calls[0]
        assert args[0] == lva_service.ListProcessesRequest()

@pytest.mark.asyncio
async def test_list_processes_async(transport: str = 'grpc_asyncio', request_type=lva_service.ListProcessesRequest):
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # Everything is optional in proto3 as far as the runtime is concerned,
    # and we are mocking out the actual API, so just send an empty request.
    request = request_type()

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.list_processes),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value =grpc_helpers_async.FakeUnaryUnaryCall(lva_service.ListProcessesResponse(
            next_page_token='next_page_token_value',
            unreachable=['unreachable_value'],
        ))
        response = await client.list_processes(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls)
        _, args, _ = call.mock_calls[0]
        assert args[0] == lva_service.ListProcessesRequest()

    # Establish that the response is the type that we expect.
    assert isinstance(response, pagers.ListProcessesAsyncPager)
    assert response.next_page_token == 'next_page_token_value'
    assert response.unreachable == ['unreachable_value']


@pytest.mark.asyncio
async def test_list_processes_async_from_dict():
    await test_list_processes_async(request_type=dict)


def test_list_processes_field_headers():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Any value that is part of the HTTP/1.1 URI should be sent as
    # a field header. Set these to a non-empty value.
    request = lva_service.ListProcessesRequest()

    request.parent = 'parent_value'

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.list_processes),
            '__call__') as call:
        call.return_value = lva_service.ListProcessesResponse()
        client.list_processes(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        assert args[0] == request

    # Establish that the field header was sent.
    _, _, kw = call.mock_calls[0]
    assert (
        'x-goog-request-params',
        'parent=parent_value',
    ) in kw['metadata']


@pytest.mark.asyncio
async def test_list_processes_field_headers_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Any value that is part of the HTTP/1.1 URI should be sent as
    # a field header. Set these to a non-empty value.
    request = lva_service.ListProcessesRequest()

    request.parent = 'parent_value'

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.list_processes),
            '__call__') as call:
        call.return_value = grpc_helpers_async.FakeUnaryUnaryCall(lva_service.ListProcessesResponse())
        await client.list_processes(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls)
        _, args, _ = call.mock_calls[0]
        assert args[0] == request

    # Establish that the field header was sent.
    _, _, kw = call.mock_calls[0]
    assert (
        'x-goog-request-params',
        'parent=parent_value',
    ) in kw['metadata']


def test_list_processes_flattened():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.list_processes),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = lva_service.ListProcessesResponse()
        # Call the method with a truthy value for each flattened field,
        # using the keyword arguments to the method.
        client.list_processes(
            parent='parent_value',
        )

        # Establish that the underlying call was made with the expected
        # request object values.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        arg = args[0].parent
        mock_val = 'parent_value'
        assert arg == mock_val


def test_list_processes_flattened_error():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Attempting to call a method with both a request object and flattened
    # fields is an error.
    with pytest.raises(ValueError):
        client.list_processes(
            lva_service.ListProcessesRequest(),
            parent='parent_value',
        )

@pytest.mark.asyncio
async def test_list_processes_flattened_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.list_processes),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = lva_service.ListProcessesResponse()

        call.return_value = grpc_helpers_async.FakeUnaryUnaryCall(lva_service.ListProcessesResponse())
        # Call the method with a truthy value for each flattened field,
        # using the keyword arguments to the method.
        response = await client.list_processes(
            parent='parent_value',
        )

        # Establish that the underlying call was made with the expected
        # request object values.
        assert len(call.mock_calls)
        _, args, _ = call.mock_calls[0]
        arg = args[0].parent
        mock_val = 'parent_value'
        assert arg == mock_val

@pytest.mark.asyncio
async def test_list_processes_flattened_error_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Attempting to call a method with both a request object and flattened
    # fields is an error.
    with pytest.raises(ValueError):
        await client.list_processes(
            lva_service.ListProcessesRequest(),
            parent='parent_value',
        )


def test_list_processes_pager(transport_name: str = "grpc"):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials,
        transport=transport_name,
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.list_processes),
            '__call__') as call:
        # Set the response to a series of pages.
        call.side_effect = (
            lva_service.ListProcessesResponse(
                processes=[
                    lva_resources.Process(),
                    lva_resources.Process(),
                    lva_resources.Process(),
                ],
                next_page_token='abc',
            ),
            lva_service.ListProcessesResponse(
                processes=[],
                next_page_token='def',
            ),
            lva_service.ListProcessesResponse(
                processes=[
                    lva_resources.Process(),
                ],
                next_page_token='ghi',
            ),
            lva_service.ListProcessesResponse(
                processes=[
                    lva_resources.Process(),
                    lva_resources.Process(),
                ],
            ),
            RuntimeError,
        )

        metadata = ()
        metadata = tuple(metadata) + (
            gapic_v1.routing_header.to_grpc_metadata((
                ('parent', ''),
            )),
        )
        pager = client.list_processes(request={})

        assert pager._metadata == metadata

        results = list(pager)
        assert len(results) == 6
        assert all(isinstance(i, lva_resources.Process)
                   for i in results)
def test_list_processes_pages(transport_name: str = "grpc"):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials,
        transport=transport_name,
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.list_processes),
            '__call__') as call:
        # Set the response to a series of pages.
        call.side_effect = (
            lva_service.ListProcessesResponse(
                processes=[
                    lva_resources.Process(),
                    lva_resources.Process(),
                    lva_resources.Process(),
                ],
                next_page_token='abc',
            ),
            lva_service.ListProcessesResponse(
                processes=[],
                next_page_token='def',
            ),
            lva_service.ListProcessesResponse(
                processes=[
                    lva_resources.Process(),
                ],
                next_page_token='ghi',
            ),
            lva_service.ListProcessesResponse(
                processes=[
                    lva_resources.Process(),
                    lva_resources.Process(),
                ],
            ),
            RuntimeError,
        )
        pages = list(client.list_processes(request={}).pages)
        for page_, token in zip(pages, ['abc','def','ghi', '']):
            assert page_.raw_page.next_page_token == token

@pytest.mark.asyncio
async def test_list_processes_async_pager():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials,
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.list_processes),
            '__call__', new_callable=mock.AsyncMock) as call:
        # Set the response to a series of pages.
        call.side_effect = (
            lva_service.ListProcessesResponse(
                processes=[
                    lva_resources.Process(),
                    lva_resources.Process(),
                    lva_resources.Process(),
                ],
                next_page_token='abc',
            ),
            lva_service.ListProcessesResponse(
                processes=[],
                next_page_token='def',
            ),
            lva_service.ListProcessesResponse(
                processes=[
                    lva_resources.Process(),
                ],
                next_page_token='ghi',
            ),
            lva_service.ListProcessesResponse(
                processes=[
                    lva_resources.Process(),
                    lva_resources.Process(),
                ],
            ),
            RuntimeError,
        )
        async_pager = await client.list_processes(request={},)
        assert async_pager.next_page_token == 'abc'
        responses = []
        async for response in async_pager: # pragma: no branch
            responses.append(response)

        assert len(responses) == 6
        assert all(isinstance(i, lva_resources.Process)
                for i in responses)


@pytest.mark.asyncio
async def test_list_processes_async_pages():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials,
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.list_processes),
            '__call__', new_callable=mock.AsyncMock) as call:
        # Set the response to a series of pages.
        call.side_effect = (
            lva_service.ListProcessesResponse(
                processes=[
                    lva_resources.Process(),
                    lva_resources.Process(),
                    lva_resources.Process(),
                ],
                next_page_token='abc',
            ),
            lva_service.ListProcessesResponse(
                processes=[],
                next_page_token='def',
            ),
            lva_service.ListProcessesResponse(
                processes=[
                    lva_resources.Process(),
                ],
                next_page_token='ghi',
            ),
            lva_service.ListProcessesResponse(
                processes=[
                    lva_resources.Process(),
                    lva_resources.Process(),
                ],
            ),
            RuntimeError,
        )
        pages = []
        # Workaround issue in python 3.9 related to code coverage by adding `# pragma: no branch`
        # See https://github.com/googleapis/gapic-generator-python/pull/1174#issuecomment-1025132372
        async for page_ in ( # pragma: no branch
            await client.list_processes(request={})
        ).pages:
            pages.append(page_)
        for page_, token in zip(pages, ['abc','def','ghi', '']):
            assert page_.raw_page.next_page_token == token

@pytest.mark.parametrize("request_type", [
  lva_service.GetProcessRequest,
  dict,
])
def test_get_process(request_type, transport: str = 'grpc'):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # Everything is optional in proto3 as far as the runtime is concerned,
    # and we are mocking out the actual API, so just send an empty request.
    request = request_type()

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.get_process),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = lva_resources.Process(
            name='name_value',
            analysis='analysis_value',
            attribute_overrides=['attribute_overrides_value'],
            run_mode=lva.RunMode.LIVE,
            event_id='event_id_value',
            batch_id='batch_id_value',
            retry_count=1214,
        )
        response = client.get_process(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        assert args[0] == lva_service.GetProcessRequest()

    # Establish that the response is the type that we expect.
    assert isinstance(response, lva_resources.Process)
    assert response.name == 'name_value'
    assert response.analysis == 'analysis_value'
    assert response.attribute_overrides == ['attribute_overrides_value']
    assert response.run_mode == lva.RunMode.LIVE
    assert response.event_id == 'event_id_value'
    assert response.batch_id == 'batch_id_value'
    assert response.retry_count == 1214


def test_get_process_empty_call():
    # This test is a coverage failsafe to make sure that totally empty calls,
    # i.e. request == None and no flattened fields passed, work.
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport='grpc',
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.get_process),
            '__call__') as call:
        client.get_process()
        call.assert_called()
        _, args, _ = call.mock_calls[0]
        assert args[0] == lva_service.GetProcessRequest()

@pytest.mark.asyncio
async def test_get_process_async(transport: str = 'grpc_asyncio', request_type=lva_service.GetProcessRequest):
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # Everything is optional in proto3 as far as the runtime is concerned,
    # and we are mocking out the actual API, so just send an empty request.
    request = request_type()

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.get_process),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value =grpc_helpers_async.FakeUnaryUnaryCall(lva_resources.Process(
            name='name_value',
            analysis='analysis_value',
            attribute_overrides=['attribute_overrides_value'],
            run_mode=lva.RunMode.LIVE,
            event_id='event_id_value',
            batch_id='batch_id_value',
            retry_count=1214,
        ))
        response = await client.get_process(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls)
        _, args, _ = call.mock_calls[0]
        assert args[0] == lva_service.GetProcessRequest()

    # Establish that the response is the type that we expect.
    assert isinstance(response, lva_resources.Process)
    assert response.name == 'name_value'
    assert response.analysis == 'analysis_value'
    assert response.attribute_overrides == ['attribute_overrides_value']
    assert response.run_mode == lva.RunMode.LIVE
    assert response.event_id == 'event_id_value'
    assert response.batch_id == 'batch_id_value'
    assert response.retry_count == 1214


@pytest.mark.asyncio
async def test_get_process_async_from_dict():
    await test_get_process_async(request_type=dict)


def test_get_process_field_headers():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Any value that is part of the HTTP/1.1 URI should be sent as
    # a field header. Set these to a non-empty value.
    request = lva_service.GetProcessRequest()

    request.name = 'name_value'

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.get_process),
            '__call__') as call:
        call.return_value = lva_resources.Process()
        client.get_process(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        assert args[0] == request

    # Establish that the field header was sent.
    _, _, kw = call.mock_calls[0]
    assert (
        'x-goog-request-params',
        'name=name_value',
    ) in kw['metadata']


@pytest.mark.asyncio
async def test_get_process_field_headers_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Any value that is part of the HTTP/1.1 URI should be sent as
    # a field header. Set these to a non-empty value.
    request = lva_service.GetProcessRequest()

    request.name = 'name_value'

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.get_process),
            '__call__') as call:
        call.return_value = grpc_helpers_async.FakeUnaryUnaryCall(lva_resources.Process())
        await client.get_process(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls)
        _, args, _ = call.mock_calls[0]
        assert args[0] == request

    # Establish that the field header was sent.
    _, _, kw = call.mock_calls[0]
    assert (
        'x-goog-request-params',
        'name=name_value',
    ) in kw['metadata']


def test_get_process_flattened():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.get_process),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = lva_resources.Process()
        # Call the method with a truthy value for each flattened field,
        # using the keyword arguments to the method.
        client.get_process(
            name='name_value',
        )

        # Establish that the underlying call was made with the expected
        # request object values.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        arg = args[0].name
        mock_val = 'name_value'
        assert arg == mock_val


def test_get_process_flattened_error():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Attempting to call a method with both a request object and flattened
    # fields is an error.
    with pytest.raises(ValueError):
        client.get_process(
            lva_service.GetProcessRequest(),
            name='name_value',
        )

@pytest.mark.asyncio
async def test_get_process_flattened_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.get_process),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = lva_resources.Process()

        call.return_value = grpc_helpers_async.FakeUnaryUnaryCall(lva_resources.Process())
        # Call the method with a truthy value for each flattened field,
        # using the keyword arguments to the method.
        response = await client.get_process(
            name='name_value',
        )

        # Establish that the underlying call was made with the expected
        # request object values.
        assert len(call.mock_calls)
        _, args, _ = call.mock_calls[0]
        arg = args[0].name
        mock_val = 'name_value'
        assert arg == mock_val

@pytest.mark.asyncio
async def test_get_process_flattened_error_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Attempting to call a method with both a request object and flattened
    # fields is an error.
    with pytest.raises(ValueError):
        await client.get_process(
            lva_service.GetProcessRequest(),
            name='name_value',
        )


@pytest.mark.parametrize("request_type", [
  lva_service.CreateProcessRequest,
  dict,
])
def test_create_process(request_type, transport: str = 'grpc'):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # Everything is optional in proto3 as far as the runtime is concerned,
    # and we are mocking out the actual API, so just send an empty request.
    request = request_type()

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.create_process),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = operations_pb2.Operation(name='operations/spam')
        response = client.create_process(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        assert args[0] == lva_service.CreateProcessRequest()

    # Establish that the response is the type that we expect.
    assert isinstance(response, future.Future)


def test_create_process_empty_call():
    # This test is a coverage failsafe to make sure that totally empty calls,
    # i.e. request == None and no flattened fields passed, work.
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport='grpc',
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.create_process),
            '__call__') as call:
        client.create_process()
        call.assert_called()
        _, args, _ = call.mock_calls[0]
        assert args[0] == lva_service.CreateProcessRequest()

@pytest.mark.asyncio
async def test_create_process_async(transport: str = 'grpc_asyncio', request_type=lva_service.CreateProcessRequest):
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # Everything is optional in proto3 as far as the runtime is concerned,
    # and we are mocking out the actual API, so just send an empty request.
    request = request_type()

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.create_process),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = grpc_helpers_async.FakeUnaryUnaryCall(
            operations_pb2.Operation(name='operations/spam')
        )
        response = await client.create_process(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls)
        _, args, _ = call.mock_calls[0]
        assert args[0] == lva_service.CreateProcessRequest()

    # Establish that the response is the type that we expect.
    assert isinstance(response, future.Future)


@pytest.mark.asyncio
async def test_create_process_async_from_dict():
    await test_create_process_async(request_type=dict)


def test_create_process_field_headers():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Any value that is part of the HTTP/1.1 URI should be sent as
    # a field header. Set these to a non-empty value.
    request = lva_service.CreateProcessRequest()

    request.parent = 'parent_value'

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.create_process),
            '__call__') as call:
        call.return_value = operations_pb2.Operation(name='operations/op')
        client.create_process(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        assert args[0] == request

    # Establish that the field header was sent.
    _, _, kw = call.mock_calls[0]
    assert (
        'x-goog-request-params',
        'parent=parent_value',
    ) in kw['metadata']


@pytest.mark.asyncio
async def test_create_process_field_headers_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Any value that is part of the HTTP/1.1 URI should be sent as
    # a field header. Set these to a non-empty value.
    request = lva_service.CreateProcessRequest()

    request.parent = 'parent_value'

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.create_process),
            '__call__') as call:
        call.return_value = grpc_helpers_async.FakeUnaryUnaryCall(operations_pb2.Operation(name='operations/op'))
        await client.create_process(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls)
        _, args, _ = call.mock_calls[0]
        assert args[0] == request

    # Establish that the field header was sent.
    _, _, kw = call.mock_calls[0]
    assert (
        'x-goog-request-params',
        'parent=parent_value',
    ) in kw['metadata']


def test_create_process_flattened():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.create_process),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = operations_pb2.Operation(name='operations/op')
        # Call the method with a truthy value for each flattened field,
        # using the keyword arguments to the method.
        client.create_process(
            parent='parent_value',
            process=lva_resources.Process(name='name_value'),
            process_id='process_id_value',
        )

        # Establish that the underlying call was made with the expected
        # request object values.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        arg = args[0].parent
        mock_val = 'parent_value'
        assert arg == mock_val
        arg = args[0].process
        mock_val = lva_resources.Process(name='name_value')
        assert arg == mock_val
        arg = args[0].process_id
        mock_val = 'process_id_value'
        assert arg == mock_val


def test_create_process_flattened_error():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Attempting to call a method with both a request object and flattened
    # fields is an error.
    with pytest.raises(ValueError):
        client.create_process(
            lva_service.CreateProcessRequest(),
            parent='parent_value',
            process=lva_resources.Process(name='name_value'),
            process_id='process_id_value',
        )

@pytest.mark.asyncio
async def test_create_process_flattened_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.create_process),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = operations_pb2.Operation(name='operations/op')

        call.return_value = grpc_helpers_async.FakeUnaryUnaryCall(
            operations_pb2.Operation(name='operations/spam')
        )
        # Call the method with a truthy value for each flattened field,
        # using the keyword arguments to the method.
        response = await client.create_process(
            parent='parent_value',
            process=lva_resources.Process(name='name_value'),
            process_id='process_id_value',
        )

        # Establish that the underlying call was made with the expected
        # request object values.
        assert len(call.mock_calls)
        _, args, _ = call.mock_calls[0]
        arg = args[0].parent
        mock_val = 'parent_value'
        assert arg == mock_val
        arg = args[0].process
        mock_val = lva_resources.Process(name='name_value')
        assert arg == mock_val
        arg = args[0].process_id
        mock_val = 'process_id_value'
        assert arg == mock_val

@pytest.mark.asyncio
async def test_create_process_flattened_error_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Attempting to call a method with both a request object and flattened
    # fields is an error.
    with pytest.raises(ValueError):
        await client.create_process(
            lva_service.CreateProcessRequest(),
            parent='parent_value',
            process=lva_resources.Process(name='name_value'),
            process_id='process_id_value',
        )


@pytest.mark.parametrize("request_type", [
  lva_service.UpdateProcessRequest,
  dict,
])
def test_update_process(request_type, transport: str = 'grpc'):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # Everything is optional in proto3 as far as the runtime is concerned,
    # and we are mocking out the actual API, so just send an empty request.
    request = request_type()

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.update_process),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = operations_pb2.Operation(name='operations/spam')
        response = client.update_process(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        assert args[0] == lva_service.UpdateProcessRequest()

    # Establish that the response is the type that we expect.
    assert isinstance(response, future.Future)


def test_update_process_empty_call():
    # This test is a coverage failsafe to make sure that totally empty calls,
    # i.e. request == None and no flattened fields passed, work.
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport='grpc',
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.update_process),
            '__call__') as call:
        client.update_process()
        call.assert_called()
        _, args, _ = call.mock_calls[0]
        assert args[0] == lva_service.UpdateProcessRequest()

@pytest.mark.asyncio
async def test_update_process_async(transport: str = 'grpc_asyncio', request_type=lva_service.UpdateProcessRequest):
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # Everything is optional in proto3 as far as the runtime is concerned,
    # and we are mocking out the actual API, so just send an empty request.
    request = request_type()

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.update_process),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = grpc_helpers_async.FakeUnaryUnaryCall(
            operations_pb2.Operation(name='operations/spam')
        )
        response = await client.update_process(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls)
        _, args, _ = call.mock_calls[0]
        assert args[0] == lva_service.UpdateProcessRequest()

    # Establish that the response is the type that we expect.
    assert isinstance(response, future.Future)


@pytest.mark.asyncio
async def test_update_process_async_from_dict():
    await test_update_process_async(request_type=dict)


def test_update_process_field_headers():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Any value that is part of the HTTP/1.1 URI should be sent as
    # a field header. Set these to a non-empty value.
    request = lva_service.UpdateProcessRequest()

    request.process.name = 'name_value'

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.update_process),
            '__call__') as call:
        call.return_value = operations_pb2.Operation(name='operations/op')
        client.update_process(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        assert args[0] == request

    # Establish that the field header was sent.
    _, _, kw = call.mock_calls[0]
    assert (
        'x-goog-request-params',
        'process.name=name_value',
    ) in kw['metadata']


@pytest.mark.asyncio
async def test_update_process_field_headers_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Any value that is part of the HTTP/1.1 URI should be sent as
    # a field header. Set these to a non-empty value.
    request = lva_service.UpdateProcessRequest()

    request.process.name = 'name_value'

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.update_process),
            '__call__') as call:
        call.return_value = grpc_helpers_async.FakeUnaryUnaryCall(operations_pb2.Operation(name='operations/op'))
        await client.update_process(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls)
        _, args, _ = call.mock_calls[0]
        assert args[0] == request

    # Establish that the field header was sent.
    _, _, kw = call.mock_calls[0]
    assert (
        'x-goog-request-params',
        'process.name=name_value',
    ) in kw['metadata']


def test_update_process_flattened():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.update_process),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = operations_pb2.Operation(name='operations/op')
        # Call the method with a truthy value for each flattened field,
        # using the keyword arguments to the method.
        client.update_process(
            process=lva_resources.Process(name='name_value'),
            update_mask=field_mask_pb2.FieldMask(paths=['paths_value']),
        )

        # Establish that the underlying call was made with the expected
        # request object values.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        arg = args[0].process
        mock_val = lva_resources.Process(name='name_value')
        assert arg == mock_val
        arg = args[0].update_mask
        mock_val = field_mask_pb2.FieldMask(paths=['paths_value'])
        assert arg == mock_val


def test_update_process_flattened_error():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Attempting to call a method with both a request object and flattened
    # fields is an error.
    with pytest.raises(ValueError):
        client.update_process(
            lva_service.UpdateProcessRequest(),
            process=lva_resources.Process(name='name_value'),
            update_mask=field_mask_pb2.FieldMask(paths=['paths_value']),
        )

@pytest.mark.asyncio
async def test_update_process_flattened_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.update_process),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = operations_pb2.Operation(name='operations/op')

        call.return_value = grpc_helpers_async.FakeUnaryUnaryCall(
            operations_pb2.Operation(name='operations/spam')
        )
        # Call the method with a truthy value for each flattened field,
        # using the keyword arguments to the method.
        response = await client.update_process(
            process=lva_resources.Process(name='name_value'),
            update_mask=field_mask_pb2.FieldMask(paths=['paths_value']),
        )

        # Establish that the underlying call was made with the expected
        # request object values.
        assert len(call.mock_calls)
        _, args, _ = call.mock_calls[0]
        arg = args[0].process
        mock_val = lva_resources.Process(name='name_value')
        assert arg == mock_val
        arg = args[0].update_mask
        mock_val = field_mask_pb2.FieldMask(paths=['paths_value'])
        assert arg == mock_val

@pytest.mark.asyncio
async def test_update_process_flattened_error_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Attempting to call a method with both a request object and flattened
    # fields is an error.
    with pytest.raises(ValueError):
        await client.update_process(
            lva_service.UpdateProcessRequest(),
            process=lva_resources.Process(name='name_value'),
            update_mask=field_mask_pb2.FieldMask(paths=['paths_value']),
        )


@pytest.mark.parametrize("request_type", [
  lva_service.DeleteProcessRequest,
  dict,
])
def test_delete_process(request_type, transport: str = 'grpc'):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # Everything is optional in proto3 as far as the runtime is concerned,
    # and we are mocking out the actual API, so just send an empty request.
    request = request_type()

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.delete_process),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = operations_pb2.Operation(name='operations/spam')
        response = client.delete_process(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        assert args[0] == lva_service.DeleteProcessRequest()

    # Establish that the response is the type that we expect.
    assert isinstance(response, future.Future)


def test_delete_process_empty_call():
    # This test is a coverage failsafe to make sure that totally empty calls,
    # i.e. request == None and no flattened fields passed, work.
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport='grpc',
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.delete_process),
            '__call__') as call:
        client.delete_process()
        call.assert_called()
        _, args, _ = call.mock_calls[0]
        assert args[0] == lva_service.DeleteProcessRequest()

@pytest.mark.asyncio
async def test_delete_process_async(transport: str = 'grpc_asyncio', request_type=lva_service.DeleteProcessRequest):
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # Everything is optional in proto3 as far as the runtime is concerned,
    # and we are mocking out the actual API, so just send an empty request.
    request = request_type()

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.delete_process),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = grpc_helpers_async.FakeUnaryUnaryCall(
            operations_pb2.Operation(name='operations/spam')
        )
        response = await client.delete_process(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls)
        _, args, _ = call.mock_calls[0]
        assert args[0] == lva_service.DeleteProcessRequest()

    # Establish that the response is the type that we expect.
    assert isinstance(response, future.Future)


@pytest.mark.asyncio
async def test_delete_process_async_from_dict():
    await test_delete_process_async(request_type=dict)


def test_delete_process_field_headers():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Any value that is part of the HTTP/1.1 URI should be sent as
    # a field header. Set these to a non-empty value.
    request = lva_service.DeleteProcessRequest()

    request.name = 'name_value'

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.delete_process),
            '__call__') as call:
        call.return_value = operations_pb2.Operation(name='operations/op')
        client.delete_process(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        assert args[0] == request

    # Establish that the field header was sent.
    _, _, kw = call.mock_calls[0]
    assert (
        'x-goog-request-params',
        'name=name_value',
    ) in kw['metadata']


@pytest.mark.asyncio
async def test_delete_process_field_headers_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Any value that is part of the HTTP/1.1 URI should be sent as
    # a field header. Set these to a non-empty value.
    request = lva_service.DeleteProcessRequest()

    request.name = 'name_value'

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.delete_process),
            '__call__') as call:
        call.return_value = grpc_helpers_async.FakeUnaryUnaryCall(operations_pb2.Operation(name='operations/op'))
        await client.delete_process(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls)
        _, args, _ = call.mock_calls[0]
        assert args[0] == request

    # Establish that the field header was sent.
    _, _, kw = call.mock_calls[0]
    assert (
        'x-goog-request-params',
        'name=name_value',
    ) in kw['metadata']


def test_delete_process_flattened():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.delete_process),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = operations_pb2.Operation(name='operations/op')
        # Call the method with a truthy value for each flattened field,
        # using the keyword arguments to the method.
        client.delete_process(
            name='name_value',
        )

        # Establish that the underlying call was made with the expected
        # request object values.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        arg = args[0].name
        mock_val = 'name_value'
        assert arg == mock_val


def test_delete_process_flattened_error():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Attempting to call a method with both a request object and flattened
    # fields is an error.
    with pytest.raises(ValueError):
        client.delete_process(
            lva_service.DeleteProcessRequest(),
            name='name_value',
        )

@pytest.mark.asyncio
async def test_delete_process_flattened_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.delete_process),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = operations_pb2.Operation(name='operations/op')

        call.return_value = grpc_helpers_async.FakeUnaryUnaryCall(
            operations_pb2.Operation(name='operations/spam')
        )
        # Call the method with a truthy value for each flattened field,
        # using the keyword arguments to the method.
        response = await client.delete_process(
            name='name_value',
        )

        # Establish that the underlying call was made with the expected
        # request object values.
        assert len(call.mock_calls)
        _, args, _ = call.mock_calls[0]
        arg = args[0].name
        mock_val = 'name_value'
        assert arg == mock_val

@pytest.mark.asyncio
async def test_delete_process_flattened_error_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Attempting to call a method with both a request object and flattened
    # fields is an error.
    with pytest.raises(ValueError):
        await client.delete_process(
            lva_service.DeleteProcessRequest(),
            name='name_value',
        )


@pytest.mark.parametrize("request_type", [
  lva_service.BatchRunProcessRequest,
  dict,
])
def test_batch_run_process(request_type, transport: str = 'grpc'):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # Everything is optional in proto3 as far as the runtime is concerned,
    # and we are mocking out the actual API, so just send an empty request.
    request = request_type()

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.batch_run_process),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = operations_pb2.Operation(name='operations/spam')
        response = client.batch_run_process(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        assert args[0] == lva_service.BatchRunProcessRequest()

    # Establish that the response is the type that we expect.
    assert isinstance(response, future.Future)


def test_batch_run_process_empty_call():
    # This test is a coverage failsafe to make sure that totally empty calls,
    # i.e. request == None and no flattened fields passed, work.
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport='grpc',
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.batch_run_process),
            '__call__') as call:
        client.batch_run_process()
        call.assert_called()
        _, args, _ = call.mock_calls[0]
        assert args[0] == lva_service.BatchRunProcessRequest()

@pytest.mark.asyncio
async def test_batch_run_process_async(transport: str = 'grpc_asyncio', request_type=lva_service.BatchRunProcessRequest):
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # Everything is optional in proto3 as far as the runtime is concerned,
    # and we are mocking out the actual API, so just send an empty request.
    request = request_type()

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.batch_run_process),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = grpc_helpers_async.FakeUnaryUnaryCall(
            operations_pb2.Operation(name='operations/spam')
        )
        response = await client.batch_run_process(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls)
        _, args, _ = call.mock_calls[0]
        assert args[0] == lva_service.BatchRunProcessRequest()

    # Establish that the response is the type that we expect.
    assert isinstance(response, future.Future)


@pytest.mark.asyncio
async def test_batch_run_process_async_from_dict():
    await test_batch_run_process_async(request_type=dict)


def test_batch_run_process_field_headers():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Any value that is part of the HTTP/1.1 URI should be sent as
    # a field header. Set these to a non-empty value.
    request = lva_service.BatchRunProcessRequest()

    request.parent = 'parent_value'

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.batch_run_process),
            '__call__') as call:
        call.return_value = operations_pb2.Operation(name='operations/op')
        client.batch_run_process(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        assert args[0] == request

    # Establish that the field header was sent.
    _, _, kw = call.mock_calls[0]
    assert (
        'x-goog-request-params',
        'parent=parent_value',
    ) in kw['metadata']


@pytest.mark.asyncio
async def test_batch_run_process_field_headers_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Any value that is part of the HTTP/1.1 URI should be sent as
    # a field header. Set these to a non-empty value.
    request = lva_service.BatchRunProcessRequest()

    request.parent = 'parent_value'

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.batch_run_process),
            '__call__') as call:
        call.return_value = grpc_helpers_async.FakeUnaryUnaryCall(operations_pb2.Operation(name='operations/op'))
        await client.batch_run_process(request)

        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls)
        _, args, _ = call.mock_calls[0]
        assert args[0] == request

    # Establish that the field header was sent.
    _, _, kw = call.mock_calls[0]
    assert (
        'x-goog-request-params',
        'parent=parent_value',
    ) in kw['metadata']


def test_batch_run_process_flattened():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.batch_run_process),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = operations_pb2.Operation(name='operations/op')
        # Call the method with a truthy value for each flattened field,
        # using the keyword arguments to the method.
        client.batch_run_process(
            parent='parent_value',
            requests=[lva_service.CreateProcessRequest(parent='parent_value')],
        )

        # Establish that the underlying call was made with the expected
        # request object values.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        arg = args[0].parent
        mock_val = 'parent_value'
        assert arg == mock_val
        arg = args[0].requests
        mock_val = [lva_service.CreateProcessRequest(parent='parent_value')]
        assert arg == mock_val


def test_batch_run_process_flattened_error():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Attempting to call a method with both a request object and flattened
    # fields is an error.
    with pytest.raises(ValueError):
        client.batch_run_process(
            lva_service.BatchRunProcessRequest(),
            parent='parent_value',
            requests=[lva_service.CreateProcessRequest(parent='parent_value')],
        )

@pytest.mark.asyncio
async def test_batch_run_process_flattened_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(
            type(client.transport.batch_run_process),
            '__call__') as call:
        # Designate an appropriate return value for the call.
        call.return_value = operations_pb2.Operation(name='operations/op')

        call.return_value = grpc_helpers_async.FakeUnaryUnaryCall(
            operations_pb2.Operation(name='operations/spam')
        )
        # Call the method with a truthy value for each flattened field,
        # using the keyword arguments to the method.
        response = await client.batch_run_process(
            parent='parent_value',
            requests=[lva_service.CreateProcessRequest(parent='parent_value')],
        )

        # Establish that the underlying call was made with the expected
        # request object values.
        assert len(call.mock_calls)
        _, args, _ = call.mock_calls[0]
        arg = args[0].parent
        mock_val = 'parent_value'
        assert arg == mock_val
        arg = args[0].requests
        mock_val = [lva_service.CreateProcessRequest(parent='parent_value')]
        assert arg == mock_val

@pytest.mark.asyncio
async def test_batch_run_process_flattened_error_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Attempting to call a method with both a request object and flattened
    # fields is an error.
    with pytest.raises(ValueError):
        await client.batch_run_process(
            lva_service.BatchRunProcessRequest(),
            parent='parent_value',
            requests=[lva_service.CreateProcessRequest(parent='parent_value')],
        )


@pytest.mark.parametrize("request_type", [
    lva_service.ResolveOperatorInfoRequest,
    dict,
])
def test_resolve_operator_info_rest(request_type):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport="rest",
    )

    # send a request that will satisfy transcoding
    request_init = {'parent': 'projects/sample1/locations/sample2'}
    request = request_type(**request_init)

    # Mock the http request call within the method and fake a response.
    with mock.patch.object(type(client.transport._session), 'request') as req:
        # Designate an appropriate value for the returned response.
        return_value = lva_service.ResolveOperatorInfoResponse(
        )

        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 200
        pb_return_value = lva_service.ResolveOperatorInfoResponse.pb(return_value)
        json_return_value = json_format.MessageToJson(pb_return_value)

        response_value._content = json_return_value.encode('UTF-8')
        req.return_value = response_value
        response = client.resolve_operator_info(request)

    # Establish that the response is the type that we expect.
    assert isinstance(response, lva_service.ResolveOperatorInfoResponse)


def test_resolve_operator_info_rest_required_fields(request_type=lva_service.ResolveOperatorInfoRequest):
    transport_class = transports.LiveVideoAnalyticsRestTransport

    request_init = {}
    request_init["parent"] = ""
    request = request_type(**request_init)
    pb_request = request_type.pb(request)
    jsonified_request = json.loads(json_format.MessageToJson(
        pb_request,
        including_default_value_fields=False,
        use_integers_for_enums=False
    ))

    # verify fields with default values are dropped

    unset_fields = transport_class(credentials=ga_credentials.AnonymousCredentials()).resolve_operator_info._get_unset_required_fields(jsonified_request)
    jsonified_request.update(unset_fields)

    # verify required fields with default values are now present

    jsonified_request["parent"] = 'parent_value'

    unset_fields = transport_class(credentials=ga_credentials.AnonymousCredentials()).resolve_operator_info._get_unset_required_fields(jsonified_request)
    jsonified_request.update(unset_fields)

    # verify required fields with non-default values are left alone
    assert "parent" in jsonified_request
    assert jsonified_request["parent"] == 'parent_value'

    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport='rest',
    )
    request = request_type(**request_init)

    # Designate an appropriate value for the returned response.
    return_value = lva_service.ResolveOperatorInfoResponse()
    # Mock the http request call within the method and fake a response.
    with mock.patch.object(Session, 'request') as req:
        # We need to mock transcode() because providing default values
        # for required fields will fail the real version if the http_options
        # expect actual values for those fields.
        with mock.patch.object(path_template, 'transcode') as transcode:
            # A uri without fields and an empty body will force all the
            # request fields to show up in the query_params.
            pb_request = request_type.pb(request)
            transcode_result = {
                'uri': 'v1/sample_method',
                'method': "post",
                'query_params': pb_request,
            }
            transcode_result['body'] = pb_request
            transcode.return_value = transcode_result

            response_value = Response()
            response_value.status_code = 200

            pb_return_value = lva_service.ResolveOperatorInfoResponse.pb(return_value)
            json_return_value = json_format.MessageToJson(pb_return_value)

            response_value._content = json_return_value.encode('UTF-8')
            req.return_value = response_value

            response = client.resolve_operator_info(request)

            expected_params = [
            ]
            actual_params = req.call_args.kwargs['params']
            assert expected_params == actual_params


def test_resolve_operator_info_rest_unset_required_fields():
    transport = transports.LiveVideoAnalyticsRestTransport(credentials=ga_credentials.AnonymousCredentials)

    unset_fields = transport.resolve_operator_info._get_unset_required_fields({})
    assert set(unset_fields) == (set(()) & set(("parent", "queries", )))


@pytest.mark.parametrize("null_interceptor", [True, False])
def test_resolve_operator_info_rest_interceptors(null_interceptor):
    transport = transports.LiveVideoAnalyticsRestTransport(
        credentials=ga_credentials.AnonymousCredentials(),
        interceptor=None if null_interceptor else transports.LiveVideoAnalyticsRestInterceptor(),
        )
    client = LiveVideoAnalyticsClient(transport=transport)
    with mock.patch.object(type(client.transport._session), "request") as req, \
         mock.patch.object(path_template, "transcode")  as transcode, \
         mock.patch.object(transports.LiveVideoAnalyticsRestInterceptor, "post_resolve_operator_info") as post, \
         mock.patch.object(transports.LiveVideoAnalyticsRestInterceptor, "pre_resolve_operator_info") as pre:
        pre.assert_not_called()
        post.assert_not_called()
        pb_message = lva_service.ResolveOperatorInfoRequest.pb(lva_service.ResolveOperatorInfoRequest())
        transcode.return_value = {
            "method": "post",
            "uri": "my_uri",
            "body": pb_message,
            "query_params": pb_message,
        }

        req.return_value = Response()
        req.return_value.status_code = 200
        req.return_value.request = PreparedRequest()
        req.return_value._content = lva_service.ResolveOperatorInfoResponse.to_json(lva_service.ResolveOperatorInfoResponse())

        request = lva_service.ResolveOperatorInfoRequest()
        metadata =[
            ("key", "val"),
            ("cephalopod", "squid"),
        ]
        pre.return_value = request, metadata
        post.return_value = lva_service.ResolveOperatorInfoResponse()

        client.resolve_operator_info(request, metadata=[("key", "val"), ("cephalopod", "squid"),])

        pre.assert_called_once()
        post.assert_called_once()


def test_resolve_operator_info_rest_bad_request(transport: str = 'rest', request_type=lva_service.ResolveOperatorInfoRequest):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # send a request that will satisfy transcoding
    request_init = {'parent': 'projects/sample1/locations/sample2'}
    request = request_type(**request_init)

    # Mock the http request call within the method and fake a BadRequest error.
    with mock.patch.object(Session, 'request') as req, pytest.raises(core_exceptions.BadRequest):
        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 400
        response_value.request = Request()
        req.return_value = response_value
        client.resolve_operator_info(request)


def test_resolve_operator_info_rest_flattened():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport="rest",
    )

    # Mock the http request call within the method and fake a response.
    with mock.patch.object(type(client.transport._session), 'request') as req:
        # Designate an appropriate value for the returned response.
        return_value = lva_service.ResolveOperatorInfoResponse()

        # get arguments that satisfy an http rule for this method
        sample_request = {'parent': 'projects/sample1/locations/sample2'}

        # get truthy value for each flattened field
        mock_args = dict(
            parent='parent_value',
            queries=[lva_service.OperatorQuery(operator='operator_value')],
        )
        mock_args.update(sample_request)

        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 200
        pb_return_value = lva_service.ResolveOperatorInfoResponse.pb(return_value)
        json_return_value = json_format.MessageToJson(pb_return_value)
        response_value._content = json_return_value.encode('UTF-8')
        req.return_value = response_value

        client.resolve_operator_info(**mock_args)

        # Establish that the underlying call was made with the expected
        # request object values.
        assert len(req.mock_calls) == 1
        _, args, _ = req.mock_calls[0]
        assert path_template.validate("%s/v1/{parent=projects/*/locations/*}:resolveOperatorInfo" % client.transport._host, args[1])


def test_resolve_operator_info_rest_flattened_error(transport: str = 'rest'):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # Attempting to call a method with both a request object and flattened
    # fields is an error.
    with pytest.raises(ValueError):
        client.resolve_operator_info(
            lva_service.ResolveOperatorInfoRequest(),
            parent='parent_value',
            queries=[lva_service.OperatorQuery(operator='operator_value')],
        )


def test_resolve_operator_info_rest_error():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport='rest'
    )


@pytest.mark.parametrize("request_type", [
    lva_service.ListOperatorsRequest,
    dict,
])
def test_list_operators_rest(request_type):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport="rest",
    )

    # send a request that will satisfy transcoding
    request_init = {'parent': 'projects/sample1/locations/sample2'}
    request = request_type(**request_init)

    # Mock the http request call within the method and fake a response.
    with mock.patch.object(type(client.transport._session), 'request') as req:
        # Designate an appropriate value for the returned response.
        return_value = lva_service.ListOperatorsResponse(
              next_page_token='next_page_token_value',
              unreachable=['unreachable_value'],
        )

        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 200
        pb_return_value = lva_service.ListOperatorsResponse.pb(return_value)
        json_return_value = json_format.MessageToJson(pb_return_value)

        response_value._content = json_return_value.encode('UTF-8')
        req.return_value = response_value
        response = client.list_operators(request)

    # Establish that the response is the type that we expect.
    assert isinstance(response, pagers.ListOperatorsPager)
    assert response.next_page_token == 'next_page_token_value'
    assert response.unreachable == ['unreachable_value']


def test_list_operators_rest_required_fields(request_type=lva_service.ListOperatorsRequest):
    transport_class = transports.LiveVideoAnalyticsRestTransport

    request_init = {}
    request_init["parent"] = ""
    request = request_type(**request_init)
    pb_request = request_type.pb(request)
    jsonified_request = json.loads(json_format.MessageToJson(
        pb_request,
        including_default_value_fields=False,
        use_integers_for_enums=False
    ))

    # verify fields with default values are dropped

    unset_fields = transport_class(credentials=ga_credentials.AnonymousCredentials()).list_operators._get_unset_required_fields(jsonified_request)
    jsonified_request.update(unset_fields)

    # verify required fields with default values are now present

    jsonified_request["parent"] = 'parent_value'

    unset_fields = transport_class(credentials=ga_credentials.AnonymousCredentials()).list_operators._get_unset_required_fields(jsonified_request)
    # Check that path parameters and body parameters are not mixing in.
    assert not set(unset_fields) - set(("filter", "order_by", "page_size", "page_token", ))
    jsonified_request.update(unset_fields)

    # verify required fields with non-default values are left alone
    assert "parent" in jsonified_request
    assert jsonified_request["parent"] == 'parent_value'

    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport='rest',
    )
    request = request_type(**request_init)

    # Designate an appropriate value for the returned response.
    return_value = lva_service.ListOperatorsResponse()
    # Mock the http request call within the method and fake a response.
    with mock.patch.object(Session, 'request') as req:
        # We need to mock transcode() because providing default values
        # for required fields will fail the real version if the http_options
        # expect actual values for those fields.
        with mock.patch.object(path_template, 'transcode') as transcode:
            # A uri without fields and an empty body will force all the
            # request fields to show up in the query_params.
            pb_request = request_type.pb(request)
            transcode_result = {
                'uri': 'v1/sample_method',
                'method': "get",
                'query_params': pb_request,
            }
            transcode.return_value = transcode_result

            response_value = Response()
            response_value.status_code = 200

            pb_return_value = lva_service.ListOperatorsResponse.pb(return_value)
            json_return_value = json_format.MessageToJson(pb_return_value)

            response_value._content = json_return_value.encode('UTF-8')
            req.return_value = response_value

            response = client.list_operators(request)

            expected_params = [
            ]
            actual_params = req.call_args.kwargs['params']
            assert expected_params == actual_params


def test_list_operators_rest_unset_required_fields():
    transport = transports.LiveVideoAnalyticsRestTransport(credentials=ga_credentials.AnonymousCredentials)

    unset_fields = transport.list_operators._get_unset_required_fields({})
    assert set(unset_fields) == (set(("filter", "orderBy", "pageSize", "pageToken", )) & set(("parent", )))


@pytest.mark.parametrize("null_interceptor", [True, False])
def test_list_operators_rest_interceptors(null_interceptor):
    transport = transports.LiveVideoAnalyticsRestTransport(
        credentials=ga_credentials.AnonymousCredentials(),
        interceptor=None if null_interceptor else transports.LiveVideoAnalyticsRestInterceptor(),
        )
    client = LiveVideoAnalyticsClient(transport=transport)
    with mock.patch.object(type(client.transport._session), "request") as req, \
         mock.patch.object(path_template, "transcode")  as transcode, \
         mock.patch.object(transports.LiveVideoAnalyticsRestInterceptor, "post_list_operators") as post, \
         mock.patch.object(transports.LiveVideoAnalyticsRestInterceptor, "pre_list_operators") as pre:
        pre.assert_not_called()
        post.assert_not_called()
        pb_message = lva_service.ListOperatorsRequest.pb(lva_service.ListOperatorsRequest())
        transcode.return_value = {
            "method": "post",
            "uri": "my_uri",
            "body": pb_message,
            "query_params": pb_message,
        }

        req.return_value = Response()
        req.return_value.status_code = 200
        req.return_value.request = PreparedRequest()
        req.return_value._content = lva_service.ListOperatorsResponse.to_json(lva_service.ListOperatorsResponse())

        request = lva_service.ListOperatorsRequest()
        metadata =[
            ("key", "val"),
            ("cephalopod", "squid"),
        ]
        pre.return_value = request, metadata
        post.return_value = lva_service.ListOperatorsResponse()

        client.list_operators(request, metadata=[("key", "val"), ("cephalopod", "squid"),])

        pre.assert_called_once()
        post.assert_called_once()


def test_list_operators_rest_bad_request(transport: str = 'rest', request_type=lva_service.ListOperatorsRequest):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # send a request that will satisfy transcoding
    request_init = {'parent': 'projects/sample1/locations/sample2'}
    request = request_type(**request_init)

    # Mock the http request call within the method and fake a BadRequest error.
    with mock.patch.object(Session, 'request') as req, pytest.raises(core_exceptions.BadRequest):
        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 400
        response_value.request = Request()
        req.return_value = response_value
        client.list_operators(request)


def test_list_operators_rest_flattened():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport="rest",
    )

    # Mock the http request call within the method and fake a response.
    with mock.patch.object(type(client.transport._session), 'request') as req:
        # Designate an appropriate value for the returned response.
        return_value = lva_service.ListOperatorsResponse()

        # get arguments that satisfy an http rule for this method
        sample_request = {'parent': 'projects/sample1/locations/sample2'}

        # get truthy value for each flattened field
        mock_args = dict(
            parent='parent_value',
        )
        mock_args.update(sample_request)

        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 200
        pb_return_value = lva_service.ListOperatorsResponse.pb(return_value)
        json_return_value = json_format.MessageToJson(pb_return_value)
        response_value._content = json_return_value.encode('UTF-8')
        req.return_value = response_value

        client.list_operators(**mock_args)

        # Establish that the underlying call was made with the expected
        # request object values.
        assert len(req.mock_calls) == 1
        _, args, _ = req.mock_calls[0]
        assert path_template.validate("%s/v1/{parent=projects/*/locations/*}/operators" % client.transport._host, args[1])


def test_list_operators_rest_flattened_error(transport: str = 'rest'):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # Attempting to call a method with both a request object and flattened
    # fields is an error.
    with pytest.raises(ValueError):
        client.list_operators(
            lva_service.ListOperatorsRequest(),
            parent='parent_value',
        )


def test_list_operators_rest_pager(transport: str = 'rest'):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # Mock the http request call within the method and fake a response.
    with mock.patch.object(Session, 'request') as req:
        # TODO(kbandes): remove this mock unless there's a good reason for it.
        #with mock.patch.object(path_template, 'transcode') as transcode:
        # Set the response as a series of pages
        response = (
            lva_service.ListOperatorsResponse(
                operators=[
                    lva_resources.Operator(),
                    lva_resources.Operator(),
                    lva_resources.Operator(),
                ],
                next_page_token='abc',
            ),
            lva_service.ListOperatorsResponse(
                operators=[],
                next_page_token='def',
            ),
            lva_service.ListOperatorsResponse(
                operators=[
                    lva_resources.Operator(),
                ],
                next_page_token='ghi',
            ),
            lva_service.ListOperatorsResponse(
                operators=[
                    lva_resources.Operator(),
                    lva_resources.Operator(),
                ],
            ),
        )
        # Two responses for two calls
        response = response + response

        # Wrap the values into proper Response objs
        response = tuple(lva_service.ListOperatorsResponse.to_json(x) for x in response)
        return_values = tuple(Response() for i in response)
        for return_val, response_val in zip(return_values, response):
            return_val._content = response_val.encode('UTF-8')
            return_val.status_code = 200
        req.side_effect = return_values

        sample_request = {'parent': 'projects/sample1/locations/sample2'}

        pager = client.list_operators(request=sample_request)

        results = list(pager)
        assert len(results) == 6
        assert all(isinstance(i, lva_resources.Operator)
                for i in results)

        pages = list(client.list_operators(request=sample_request).pages)
        for page_, token in zip(pages, ['abc','def','ghi', '']):
            assert page_.raw_page.next_page_token == token


@pytest.mark.parametrize("request_type", [
    lva_service.GetOperatorRequest,
    dict,
])
def test_get_operator_rest(request_type):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport="rest",
    )

    # send a request that will satisfy transcoding
    request_init = {'name': 'projects/sample1/locations/sample2/operators/sample3'}
    request = request_type(**request_init)

    # Mock the http request call within the method and fake a response.
    with mock.patch.object(type(client.transport._session), 'request') as req:
        # Designate an appropriate value for the returned response.
        return_value = lva_resources.Operator(
              name='name_value',
              docker_image='docker_image_value',
        )

        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 200
        pb_return_value = lva_resources.Operator.pb(return_value)
        json_return_value = json_format.MessageToJson(pb_return_value)

        response_value._content = json_return_value.encode('UTF-8')
        req.return_value = response_value
        response = client.get_operator(request)

    # Establish that the response is the type that we expect.
    assert isinstance(response, lva_resources.Operator)
    assert response.name == 'name_value'
    assert response.docker_image == 'docker_image_value'


def test_get_operator_rest_required_fields(request_type=lva_service.GetOperatorRequest):
    transport_class = transports.LiveVideoAnalyticsRestTransport

    request_init = {}
    request_init["name"] = ""
    request = request_type(**request_init)
    pb_request = request_type.pb(request)
    jsonified_request = json.loads(json_format.MessageToJson(
        pb_request,
        including_default_value_fields=False,
        use_integers_for_enums=False
    ))

    # verify fields with default values are dropped

    unset_fields = transport_class(credentials=ga_credentials.AnonymousCredentials()).get_operator._get_unset_required_fields(jsonified_request)
    jsonified_request.update(unset_fields)

    # verify required fields with default values are now present

    jsonified_request["name"] = 'name_value'

    unset_fields = transport_class(credentials=ga_credentials.AnonymousCredentials()).get_operator._get_unset_required_fields(jsonified_request)
    jsonified_request.update(unset_fields)

    # verify required fields with non-default values are left alone
    assert "name" in jsonified_request
    assert jsonified_request["name"] == 'name_value'

    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport='rest',
    )
    request = request_type(**request_init)

    # Designate an appropriate value for the returned response.
    return_value = lva_resources.Operator()
    # Mock the http request call within the method and fake a response.
    with mock.patch.object(Session, 'request') as req:
        # We need to mock transcode() because providing default values
        # for required fields will fail the real version if the http_options
        # expect actual values for those fields.
        with mock.patch.object(path_template, 'transcode') as transcode:
            # A uri without fields and an empty body will force all the
            # request fields to show up in the query_params.
            pb_request = request_type.pb(request)
            transcode_result = {
                'uri': 'v1/sample_method',
                'method': "get",
                'query_params': pb_request,
            }
            transcode.return_value = transcode_result

            response_value = Response()
            response_value.status_code = 200

            pb_return_value = lva_resources.Operator.pb(return_value)
            json_return_value = json_format.MessageToJson(pb_return_value)

            response_value._content = json_return_value.encode('UTF-8')
            req.return_value = response_value

            response = client.get_operator(request)

            expected_params = [
            ]
            actual_params = req.call_args.kwargs['params']
            assert expected_params == actual_params


def test_get_operator_rest_unset_required_fields():
    transport = transports.LiveVideoAnalyticsRestTransport(credentials=ga_credentials.AnonymousCredentials)

    unset_fields = transport.get_operator._get_unset_required_fields({})
    assert set(unset_fields) == (set(()) & set(("name", )))


@pytest.mark.parametrize("null_interceptor", [True, False])
def test_get_operator_rest_interceptors(null_interceptor):
    transport = transports.LiveVideoAnalyticsRestTransport(
        credentials=ga_credentials.AnonymousCredentials(),
        interceptor=None if null_interceptor else transports.LiveVideoAnalyticsRestInterceptor(),
        )
    client = LiveVideoAnalyticsClient(transport=transport)
    with mock.patch.object(type(client.transport._session), "request") as req, \
         mock.patch.object(path_template, "transcode")  as transcode, \
         mock.patch.object(transports.LiveVideoAnalyticsRestInterceptor, "post_get_operator") as post, \
         mock.patch.object(transports.LiveVideoAnalyticsRestInterceptor, "pre_get_operator") as pre:
        pre.assert_not_called()
        post.assert_not_called()
        pb_message = lva_service.GetOperatorRequest.pb(lva_service.GetOperatorRequest())
        transcode.return_value = {
            "method": "post",
            "uri": "my_uri",
            "body": pb_message,
            "query_params": pb_message,
        }

        req.return_value = Response()
        req.return_value.status_code = 200
        req.return_value.request = PreparedRequest()
        req.return_value._content = lva_resources.Operator.to_json(lva_resources.Operator())

        request = lva_service.GetOperatorRequest()
        metadata =[
            ("key", "val"),
            ("cephalopod", "squid"),
        ]
        pre.return_value = request, metadata
        post.return_value = lva_resources.Operator()

        client.get_operator(request, metadata=[("key", "val"), ("cephalopod", "squid"),])

        pre.assert_called_once()
        post.assert_called_once()


def test_get_operator_rest_bad_request(transport: str = 'rest', request_type=lva_service.GetOperatorRequest):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # send a request that will satisfy transcoding
    request_init = {'name': 'projects/sample1/locations/sample2/operators/sample3'}
    request = request_type(**request_init)

    # Mock the http request call within the method and fake a BadRequest error.
    with mock.patch.object(Session, 'request') as req, pytest.raises(core_exceptions.BadRequest):
        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 400
        response_value.request = Request()
        req.return_value = response_value
        client.get_operator(request)


def test_get_operator_rest_flattened():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport="rest",
    )

    # Mock the http request call within the method and fake a response.
    with mock.patch.object(type(client.transport._session), 'request') as req:
        # Designate an appropriate value for the returned response.
        return_value = lva_resources.Operator()

        # get arguments that satisfy an http rule for this method
        sample_request = {'name': 'projects/sample1/locations/sample2/operators/sample3'}

        # get truthy value for each flattened field
        mock_args = dict(
            name='name_value',
        )
        mock_args.update(sample_request)

        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 200
        pb_return_value = lva_resources.Operator.pb(return_value)
        json_return_value = json_format.MessageToJson(pb_return_value)
        response_value._content = json_return_value.encode('UTF-8')
        req.return_value = response_value

        client.get_operator(**mock_args)

        # Establish that the underlying call was made with the expected
        # request object values.
        assert len(req.mock_calls) == 1
        _, args, _ = req.mock_calls[0]
        assert path_template.validate("%s/v1/{name=projects/*/locations/*/operators/*}" % client.transport._host, args[1])


def test_get_operator_rest_flattened_error(transport: str = 'rest'):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # Attempting to call a method with both a request object and flattened
    # fields is an error.
    with pytest.raises(ValueError):
        client.get_operator(
            lva_service.GetOperatorRequest(),
            name='name_value',
        )


def test_get_operator_rest_error():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport='rest'
    )


@pytest.mark.parametrize("request_type", [
    lva_service.CreateOperatorRequest,
    dict,
])
def test_create_operator_rest(request_type):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport="rest",
    )

    # send a request that will satisfy transcoding
    request_init = {'parent': 'projects/sample1/locations/sample2'}
    request_init["operator"] = {'name': 'name_value', 'create_time': {'seconds': 751, 'nanos': 543}, 'update_time': {}, 'labels': {}, 'operator_definition': {'operator': 'operator_value', 'input_args': [{'argument': 'argument_value', 'type_': 'type__value'}], 'output_args': {}, 'attributes': [{'attribute': 'attribute_value', 'type_': 'type__value', 'default_value': {'i': 105, 'f': 0.10200000000000001, 'b': True, 's': b's_blob'}}], 'resources': {'cpu': 'cpu_value', 'memory': 'memory_value', 'gpus': 447, 'latency_budget_ms': 1801}, 'short_description': 'short_description_value', 'description': 'description_value'}, 'docker_image': 'docker_image_value'}
    request = request_type(**request_init)

    # Mock the http request call within the method and fake a response.
    with mock.patch.object(type(client.transport._session), 'request') as req:
        # Designate an appropriate value for the returned response.
        return_value = operations_pb2.Operation(name='operations/spam')

        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 200
        json_return_value = json_format.MessageToJson(return_value)

        response_value._content = json_return_value.encode('UTF-8')
        req.return_value = response_value
        response = client.create_operator(request)

    # Establish that the response is the type that we expect.
    assert response.operation.name == "operations/spam"


def test_create_operator_rest_required_fields(request_type=lva_service.CreateOperatorRequest):
    transport_class = transports.LiveVideoAnalyticsRestTransport

    request_init = {}
    request_init["parent"] = ""
    request_init["operator_id"] = ""
    request = request_type(**request_init)
    pb_request = request_type.pb(request)
    jsonified_request = json.loads(json_format.MessageToJson(
        pb_request,
        including_default_value_fields=False,
        use_integers_for_enums=False
    ))

    # verify fields with default values are dropped
    assert "operatorId" not in jsonified_request

    unset_fields = transport_class(credentials=ga_credentials.AnonymousCredentials()).create_operator._get_unset_required_fields(jsonified_request)
    jsonified_request.update(unset_fields)

    # verify required fields with default values are now present
    assert "operatorId" in jsonified_request
    assert jsonified_request["operatorId"] == request_init["operator_id"]

    jsonified_request["parent"] = 'parent_value'
    jsonified_request["operatorId"] = 'operator_id_value'

    unset_fields = transport_class(credentials=ga_credentials.AnonymousCredentials()).create_operator._get_unset_required_fields(jsonified_request)
    # Check that path parameters and body parameters are not mixing in.
    assert not set(unset_fields) - set(("operator_id", "request_id", ))
    jsonified_request.update(unset_fields)

    # verify required fields with non-default values are left alone
    assert "parent" in jsonified_request
    assert jsonified_request["parent"] == 'parent_value'
    assert "operatorId" in jsonified_request
    assert jsonified_request["operatorId"] == 'operator_id_value'

    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport='rest',
    )
    request = request_type(**request_init)

    # Designate an appropriate value for the returned response.
    return_value = operations_pb2.Operation(name='operations/spam')
    # Mock the http request call within the method and fake a response.
    with mock.patch.object(Session, 'request') as req:
        # We need to mock transcode() because providing default values
        # for required fields will fail the real version if the http_options
        # expect actual values for those fields.
        with mock.patch.object(path_template, 'transcode') as transcode:
            # A uri without fields and an empty body will force all the
            # request fields to show up in the query_params.
            pb_request = request_type.pb(request)
            transcode_result = {
                'uri': 'v1/sample_method',
                'method': "post",
                'query_params': pb_request,
            }
            transcode_result['body'] = pb_request
            transcode.return_value = transcode_result

            response_value = Response()
            response_value.status_code = 200
            json_return_value = json_format.MessageToJson(return_value)

            response_value._content = json_return_value.encode('UTF-8')
            req.return_value = response_value

            response = client.create_operator(request)

            expected_params = [
                (
                    "operatorId",
                    "",
                ),
            ]
            actual_params = req.call_args.kwargs['params']
            assert expected_params == actual_params


def test_create_operator_rest_unset_required_fields():
    transport = transports.LiveVideoAnalyticsRestTransport(credentials=ga_credentials.AnonymousCredentials)

    unset_fields = transport.create_operator._get_unset_required_fields({})
    assert set(unset_fields) == (set(("operatorId", "requestId", )) & set(("parent", "operatorId", "operator", )))


@pytest.mark.parametrize("null_interceptor", [True, False])
def test_create_operator_rest_interceptors(null_interceptor):
    transport = transports.LiveVideoAnalyticsRestTransport(
        credentials=ga_credentials.AnonymousCredentials(),
        interceptor=None if null_interceptor else transports.LiveVideoAnalyticsRestInterceptor(),
        )
    client = LiveVideoAnalyticsClient(transport=transport)
    with mock.patch.object(type(client.transport._session), "request") as req, \
         mock.patch.object(path_template, "transcode")  as transcode, \
         mock.patch.object(operation.Operation, "_set_result_from_operation"), \
         mock.patch.object(transports.LiveVideoAnalyticsRestInterceptor, "post_create_operator") as post, \
         mock.patch.object(transports.LiveVideoAnalyticsRestInterceptor, "pre_create_operator") as pre:
        pre.assert_not_called()
        post.assert_not_called()
        pb_message = lva_service.CreateOperatorRequest.pb(lva_service.CreateOperatorRequest())
        transcode.return_value = {
            "method": "post",
            "uri": "my_uri",
            "body": pb_message,
            "query_params": pb_message,
        }

        req.return_value = Response()
        req.return_value.status_code = 200
        req.return_value.request = PreparedRequest()
        req.return_value._content = json_format.MessageToJson(operations_pb2.Operation())

        request = lva_service.CreateOperatorRequest()
        metadata =[
            ("key", "val"),
            ("cephalopod", "squid"),
        ]
        pre.return_value = request, metadata
        post.return_value = operations_pb2.Operation()

        client.create_operator(request, metadata=[("key", "val"), ("cephalopod", "squid"),])

        pre.assert_called_once()
        post.assert_called_once()


def test_create_operator_rest_bad_request(transport: str = 'rest', request_type=lva_service.CreateOperatorRequest):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # send a request that will satisfy transcoding
    request_init = {'parent': 'projects/sample1/locations/sample2'}
    request_init["operator"] = {'name': 'name_value', 'create_time': {'seconds': 751, 'nanos': 543}, 'update_time': {}, 'labels': {}, 'operator_definition': {'operator': 'operator_value', 'input_args': [{'argument': 'argument_value', 'type_': 'type__value'}], 'output_args': {}, 'attributes': [{'attribute': 'attribute_value', 'type_': 'type__value', 'default_value': {'i': 105, 'f': 0.10200000000000001, 'b': True, 's': b's_blob'}}], 'resources': {'cpu': 'cpu_value', 'memory': 'memory_value', 'gpus': 447, 'latency_budget_ms': 1801}, 'short_description': 'short_description_value', 'description': 'description_value'}, 'docker_image': 'docker_image_value'}
    request = request_type(**request_init)

    # Mock the http request call within the method and fake a BadRequest error.
    with mock.patch.object(Session, 'request') as req, pytest.raises(core_exceptions.BadRequest):
        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 400
        response_value.request = Request()
        req.return_value = response_value
        client.create_operator(request)


def test_create_operator_rest_flattened():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport="rest",
    )

    # Mock the http request call within the method and fake a response.
    with mock.patch.object(type(client.transport._session), 'request') as req:
        # Designate an appropriate value for the returned response.
        return_value = operations_pb2.Operation(name='operations/spam')

        # get arguments that satisfy an http rule for this method
        sample_request = {'parent': 'projects/sample1/locations/sample2'}

        # get truthy value for each flattened field
        mock_args = dict(
            parent='parent_value',
            operator=lva_resources.Operator(name='name_value'),
            operator_id='operator_id_value',
        )
        mock_args.update(sample_request)

        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 200
        json_return_value = json_format.MessageToJson(return_value)
        response_value._content = json_return_value.encode('UTF-8')
        req.return_value = response_value

        client.create_operator(**mock_args)

        # Establish that the underlying call was made with the expected
        # request object values.
        assert len(req.mock_calls) == 1
        _, args, _ = req.mock_calls[0]
        assert path_template.validate("%s/v1/{parent=projects/*/locations/*}/operators" % client.transport._host, args[1])


def test_create_operator_rest_flattened_error(transport: str = 'rest'):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # Attempting to call a method with both a request object and flattened
    # fields is an error.
    with pytest.raises(ValueError):
        client.create_operator(
            lva_service.CreateOperatorRequest(),
            parent='parent_value',
            operator=lva_resources.Operator(name='name_value'),
            operator_id='operator_id_value',
        )


def test_create_operator_rest_error():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport='rest'
    )


@pytest.mark.parametrize("request_type", [
    lva_service.UpdateOperatorRequest,
    dict,
])
def test_update_operator_rest(request_type):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport="rest",
    )

    # send a request that will satisfy transcoding
    request_init = {'operator': {'name': 'projects/sample1/locations/sample2/operators/sample3'}}
    request_init["operator"] = {'name': 'projects/sample1/locations/sample2/operators/sample3', 'create_time': {'seconds': 751, 'nanos': 543}, 'update_time': {}, 'labels': {}, 'operator_definition': {'operator': 'operator_value', 'input_args': [{'argument': 'argument_value', 'type_': 'type__value'}], 'output_args': {}, 'attributes': [{'attribute': 'attribute_value', 'type_': 'type__value', 'default_value': {'i': 105, 'f': 0.10200000000000001, 'b': True, 's': b's_blob'}}], 'resources': {'cpu': 'cpu_value', 'memory': 'memory_value', 'gpus': 447, 'latency_budget_ms': 1801}, 'short_description': 'short_description_value', 'description': 'description_value'}, 'docker_image': 'docker_image_value'}
    request = request_type(**request_init)

    # Mock the http request call within the method and fake a response.
    with mock.patch.object(type(client.transport._session), 'request') as req:
        # Designate an appropriate value for the returned response.
        return_value = operations_pb2.Operation(name='operations/spam')

        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 200
        json_return_value = json_format.MessageToJson(return_value)

        response_value._content = json_return_value.encode('UTF-8')
        req.return_value = response_value
        response = client.update_operator(request)

    # Establish that the response is the type that we expect.
    assert response.operation.name == "operations/spam"


def test_update_operator_rest_required_fields(request_type=lva_service.UpdateOperatorRequest):
    transport_class = transports.LiveVideoAnalyticsRestTransport

    request_init = {}
    request = request_type(**request_init)
    pb_request = request_type.pb(request)
    jsonified_request = json.loads(json_format.MessageToJson(
        pb_request,
        including_default_value_fields=False,
        use_integers_for_enums=False
    ))

    # verify fields with default values are dropped

    unset_fields = transport_class(credentials=ga_credentials.AnonymousCredentials()).update_operator._get_unset_required_fields(jsonified_request)
    jsonified_request.update(unset_fields)

    # verify required fields with default values are now present

    unset_fields = transport_class(credentials=ga_credentials.AnonymousCredentials()).update_operator._get_unset_required_fields(jsonified_request)
    # Check that path parameters and body parameters are not mixing in.
    assert not set(unset_fields) - set(("request_id", "update_mask", ))
    jsonified_request.update(unset_fields)

    # verify required fields with non-default values are left alone

    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport='rest',
    )
    request = request_type(**request_init)

    # Designate an appropriate value for the returned response.
    return_value = operations_pb2.Operation(name='operations/spam')
    # Mock the http request call within the method and fake a response.
    with mock.patch.object(Session, 'request') as req:
        # We need to mock transcode() because providing default values
        # for required fields will fail the real version if the http_options
        # expect actual values for those fields.
        with mock.patch.object(path_template, 'transcode') as transcode:
            # A uri without fields and an empty body will force all the
            # request fields to show up in the query_params.
            pb_request = request_type.pb(request)
            transcode_result = {
                'uri': 'v1/sample_method',
                'method': "patch",
                'query_params': pb_request,
            }
            transcode_result['body'] = pb_request
            transcode.return_value = transcode_result

            response_value = Response()
            response_value.status_code = 200
            json_return_value = json_format.MessageToJson(return_value)

            response_value._content = json_return_value.encode('UTF-8')
            req.return_value = response_value

            response = client.update_operator(request)

            expected_params = [
            ]
            actual_params = req.call_args.kwargs['params']
            assert expected_params == actual_params


def test_update_operator_rest_unset_required_fields():
    transport = transports.LiveVideoAnalyticsRestTransport(credentials=ga_credentials.AnonymousCredentials)

    unset_fields = transport.update_operator._get_unset_required_fields({})
    assert set(unset_fields) == (set(("requestId", "updateMask", )) & set(("updateMask", "operator", )))


@pytest.mark.parametrize("null_interceptor", [True, False])
def test_update_operator_rest_interceptors(null_interceptor):
    transport = transports.LiveVideoAnalyticsRestTransport(
        credentials=ga_credentials.AnonymousCredentials(),
        interceptor=None if null_interceptor else transports.LiveVideoAnalyticsRestInterceptor(),
        )
    client = LiveVideoAnalyticsClient(transport=transport)
    with mock.patch.object(type(client.transport._session), "request") as req, \
         mock.patch.object(path_template, "transcode")  as transcode, \
         mock.patch.object(operation.Operation, "_set_result_from_operation"), \
         mock.patch.object(transports.LiveVideoAnalyticsRestInterceptor, "post_update_operator") as post, \
         mock.patch.object(transports.LiveVideoAnalyticsRestInterceptor, "pre_update_operator") as pre:
        pre.assert_not_called()
        post.assert_not_called()
        pb_message = lva_service.UpdateOperatorRequest.pb(lva_service.UpdateOperatorRequest())
        transcode.return_value = {
            "method": "post",
            "uri": "my_uri",
            "body": pb_message,
            "query_params": pb_message,
        }

        req.return_value = Response()
        req.return_value.status_code = 200
        req.return_value.request = PreparedRequest()
        req.return_value._content = json_format.MessageToJson(operations_pb2.Operation())

        request = lva_service.UpdateOperatorRequest()
        metadata =[
            ("key", "val"),
            ("cephalopod", "squid"),
        ]
        pre.return_value = request, metadata
        post.return_value = operations_pb2.Operation()

        client.update_operator(request, metadata=[("key", "val"), ("cephalopod", "squid"),])

        pre.assert_called_once()
        post.assert_called_once()


def test_update_operator_rest_bad_request(transport: str = 'rest', request_type=lva_service.UpdateOperatorRequest):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # send a request that will satisfy transcoding
    request_init = {'operator': {'name': 'projects/sample1/locations/sample2/operators/sample3'}}
    request_init["operator"] = {'name': 'projects/sample1/locations/sample2/operators/sample3', 'create_time': {'seconds': 751, 'nanos': 543}, 'update_time': {}, 'labels': {}, 'operator_definition': {'operator': 'operator_value', 'input_args': [{'argument': 'argument_value', 'type_': 'type__value'}], 'output_args': {}, 'attributes': [{'attribute': 'attribute_value', 'type_': 'type__value', 'default_value': {'i': 105, 'f': 0.10200000000000001, 'b': True, 's': b's_blob'}}], 'resources': {'cpu': 'cpu_value', 'memory': 'memory_value', 'gpus': 447, 'latency_budget_ms': 1801}, 'short_description': 'short_description_value', 'description': 'description_value'}, 'docker_image': 'docker_image_value'}
    request = request_type(**request_init)

    # Mock the http request call within the method and fake a BadRequest error.
    with mock.patch.object(Session, 'request') as req, pytest.raises(core_exceptions.BadRequest):
        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 400
        response_value.request = Request()
        req.return_value = response_value
        client.update_operator(request)


def test_update_operator_rest_flattened():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport="rest",
    )

    # Mock the http request call within the method and fake a response.
    with mock.patch.object(type(client.transport._session), 'request') as req:
        # Designate an appropriate value for the returned response.
        return_value = operations_pb2.Operation(name='operations/spam')

        # get arguments that satisfy an http rule for this method
        sample_request = {'operator': {'name': 'projects/sample1/locations/sample2/operators/sample3'}}

        # get truthy value for each flattened field
        mock_args = dict(
            operator=lva_resources.Operator(name='name_value'),
            update_mask=field_mask_pb2.FieldMask(paths=['paths_value']),
        )
        mock_args.update(sample_request)

        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 200
        json_return_value = json_format.MessageToJson(return_value)
        response_value._content = json_return_value.encode('UTF-8')
        req.return_value = response_value

        client.update_operator(**mock_args)

        # Establish that the underlying call was made with the expected
        # request object values.
        assert len(req.mock_calls) == 1
        _, args, _ = req.mock_calls[0]
        assert path_template.validate("%s/v1/{operator.name=projects/*/locations/*/operators/*}" % client.transport._host, args[1])


def test_update_operator_rest_flattened_error(transport: str = 'rest'):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # Attempting to call a method with both a request object and flattened
    # fields is an error.
    with pytest.raises(ValueError):
        client.update_operator(
            lva_service.UpdateOperatorRequest(),
            operator=lva_resources.Operator(name='name_value'),
            update_mask=field_mask_pb2.FieldMask(paths=['paths_value']),
        )


def test_update_operator_rest_error():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport='rest'
    )


@pytest.mark.parametrize("request_type", [
    lva_service.DeleteOperatorRequest,
    dict,
])
def test_delete_operator_rest(request_type):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport="rest",
    )

    # send a request that will satisfy transcoding
    request_init = {'name': 'projects/sample1/locations/sample2/operators/sample3'}
    request = request_type(**request_init)

    # Mock the http request call within the method and fake a response.
    with mock.patch.object(type(client.transport._session), 'request') as req:
        # Designate an appropriate value for the returned response.
        return_value = operations_pb2.Operation(name='operations/spam')

        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 200
        json_return_value = json_format.MessageToJson(return_value)

        response_value._content = json_return_value.encode('UTF-8')
        req.return_value = response_value
        response = client.delete_operator(request)

    # Establish that the response is the type that we expect.
    assert response.operation.name == "operations/spam"


def test_delete_operator_rest_required_fields(request_type=lva_service.DeleteOperatorRequest):
    transport_class = transports.LiveVideoAnalyticsRestTransport

    request_init = {}
    request_init["name"] = ""
    request = request_type(**request_init)
    pb_request = request_type.pb(request)
    jsonified_request = json.loads(json_format.MessageToJson(
        pb_request,
        including_default_value_fields=False,
        use_integers_for_enums=False
    ))

    # verify fields with default values are dropped

    unset_fields = transport_class(credentials=ga_credentials.AnonymousCredentials()).delete_operator._get_unset_required_fields(jsonified_request)
    jsonified_request.update(unset_fields)

    # verify required fields with default values are now present

    jsonified_request["name"] = 'name_value'

    unset_fields = transport_class(credentials=ga_credentials.AnonymousCredentials()).delete_operator._get_unset_required_fields(jsonified_request)
    # Check that path parameters and body parameters are not mixing in.
    assert not set(unset_fields) - set(("request_id", ))
    jsonified_request.update(unset_fields)

    # verify required fields with non-default values are left alone
    assert "name" in jsonified_request
    assert jsonified_request["name"] == 'name_value'

    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport='rest',
    )
    request = request_type(**request_init)

    # Designate an appropriate value for the returned response.
    return_value = operations_pb2.Operation(name='operations/spam')
    # Mock the http request call within the method and fake a response.
    with mock.patch.object(Session, 'request') as req:
        # We need to mock transcode() because providing default values
        # for required fields will fail the real version if the http_options
        # expect actual values for those fields.
        with mock.patch.object(path_template, 'transcode') as transcode:
            # A uri without fields and an empty body will force all the
            # request fields to show up in the query_params.
            pb_request = request_type.pb(request)
            transcode_result = {
                'uri': 'v1/sample_method',
                'method': "delete",
                'query_params': pb_request,
            }
            transcode.return_value = transcode_result

            response_value = Response()
            response_value.status_code = 200
            json_return_value = json_format.MessageToJson(return_value)

            response_value._content = json_return_value.encode('UTF-8')
            req.return_value = response_value

            response = client.delete_operator(request)

            expected_params = [
            ]
            actual_params = req.call_args.kwargs['params']
            assert expected_params == actual_params


def test_delete_operator_rest_unset_required_fields():
    transport = transports.LiveVideoAnalyticsRestTransport(credentials=ga_credentials.AnonymousCredentials)

    unset_fields = transport.delete_operator._get_unset_required_fields({})
    assert set(unset_fields) == (set(("requestId", )) & set(("name", )))


@pytest.mark.parametrize("null_interceptor", [True, False])
def test_delete_operator_rest_interceptors(null_interceptor):
    transport = transports.LiveVideoAnalyticsRestTransport(
        credentials=ga_credentials.AnonymousCredentials(),
        interceptor=None if null_interceptor else transports.LiveVideoAnalyticsRestInterceptor(),
        )
    client = LiveVideoAnalyticsClient(transport=transport)
    with mock.patch.object(type(client.transport._session), "request") as req, \
         mock.patch.object(path_template, "transcode")  as transcode, \
         mock.patch.object(operation.Operation, "_set_result_from_operation"), \
         mock.patch.object(transports.LiveVideoAnalyticsRestInterceptor, "post_delete_operator") as post, \
         mock.patch.object(transports.LiveVideoAnalyticsRestInterceptor, "pre_delete_operator") as pre:
        pre.assert_not_called()
        post.assert_not_called()
        pb_message = lva_service.DeleteOperatorRequest.pb(lva_service.DeleteOperatorRequest())
        transcode.return_value = {
            "method": "post",
            "uri": "my_uri",
            "body": pb_message,
            "query_params": pb_message,
        }

        req.return_value = Response()
        req.return_value.status_code = 200
        req.return_value.request = PreparedRequest()
        req.return_value._content = json_format.MessageToJson(operations_pb2.Operation())

        request = lva_service.DeleteOperatorRequest()
        metadata =[
            ("key", "val"),
            ("cephalopod", "squid"),
        ]
        pre.return_value = request, metadata
        post.return_value = operations_pb2.Operation()

        client.delete_operator(request, metadata=[("key", "val"), ("cephalopod", "squid"),])

        pre.assert_called_once()
        post.assert_called_once()


def test_delete_operator_rest_bad_request(transport: str = 'rest', request_type=lva_service.DeleteOperatorRequest):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # send a request that will satisfy transcoding
    request_init = {'name': 'projects/sample1/locations/sample2/operators/sample3'}
    request = request_type(**request_init)

    # Mock the http request call within the method and fake a BadRequest error.
    with mock.patch.object(Session, 'request') as req, pytest.raises(core_exceptions.BadRequest):
        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 400
        response_value.request = Request()
        req.return_value = response_value
        client.delete_operator(request)


def test_delete_operator_rest_flattened():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport="rest",
    )

    # Mock the http request call within the method and fake a response.
    with mock.patch.object(type(client.transport._session), 'request') as req:
        # Designate an appropriate value for the returned response.
        return_value = operations_pb2.Operation(name='operations/spam')

        # get arguments that satisfy an http rule for this method
        sample_request = {'name': 'projects/sample1/locations/sample2/operators/sample3'}

        # get truthy value for each flattened field
        mock_args = dict(
            name='name_value',
        )
        mock_args.update(sample_request)

        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 200
        json_return_value = json_format.MessageToJson(return_value)
        response_value._content = json_return_value.encode('UTF-8')
        req.return_value = response_value

        client.delete_operator(**mock_args)

        # Establish that the underlying call was made with the expected
        # request object values.
        assert len(req.mock_calls) == 1
        _, args, _ = req.mock_calls[0]
        assert path_template.validate("%s/v1/{name=projects/*/locations/*/operators/*}" % client.transport._host, args[1])


def test_delete_operator_rest_flattened_error(transport: str = 'rest'):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # Attempting to call a method with both a request object and flattened
    # fields is an error.
    with pytest.raises(ValueError):
        client.delete_operator(
            lva_service.DeleteOperatorRequest(),
            name='name_value',
        )


def test_delete_operator_rest_error():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport='rest'
    )


@pytest.mark.parametrize("request_type", [
    lva_service.ListAnalysesRequest,
    dict,
])
def test_list_analyses_rest(request_type):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport="rest",
    )

    # send a request that will satisfy transcoding
    request_init = {'parent': 'projects/sample1/locations/sample2/clusters/sample3'}
    request = request_type(**request_init)

    # Mock the http request call within the method and fake a response.
    with mock.patch.object(type(client.transport._session), 'request') as req:
        # Designate an appropriate value for the returned response.
        return_value = lva_service.ListAnalysesResponse(
              next_page_token='next_page_token_value',
              unreachable=['unreachable_value'],
        )

        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 200
        pb_return_value = lva_service.ListAnalysesResponse.pb(return_value)
        json_return_value = json_format.MessageToJson(pb_return_value)

        response_value._content = json_return_value.encode('UTF-8')
        req.return_value = response_value
        response = client.list_analyses(request)

    # Establish that the response is the type that we expect.
    assert isinstance(response, pagers.ListAnalysesPager)
    assert response.next_page_token == 'next_page_token_value'
    assert response.unreachable == ['unreachable_value']


def test_list_analyses_rest_required_fields(request_type=lva_service.ListAnalysesRequest):
    transport_class = transports.LiveVideoAnalyticsRestTransport

    request_init = {}
    request_init["parent"] = ""
    request = request_type(**request_init)
    pb_request = request_type.pb(request)
    jsonified_request = json.loads(json_format.MessageToJson(
        pb_request,
        including_default_value_fields=False,
        use_integers_for_enums=False
    ))

    # verify fields with default values are dropped

    unset_fields = transport_class(credentials=ga_credentials.AnonymousCredentials()).list_analyses._get_unset_required_fields(jsonified_request)
    jsonified_request.update(unset_fields)

    # verify required fields with default values are now present

    jsonified_request["parent"] = 'parent_value'

    unset_fields = transport_class(credentials=ga_credentials.AnonymousCredentials()).list_analyses._get_unset_required_fields(jsonified_request)
    # Check that path parameters and body parameters are not mixing in.
    assert not set(unset_fields) - set(("filter", "order_by", "page_size", "page_token", ))
    jsonified_request.update(unset_fields)

    # verify required fields with non-default values are left alone
    assert "parent" in jsonified_request
    assert jsonified_request["parent"] == 'parent_value'

    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport='rest',
    )
    request = request_type(**request_init)

    # Designate an appropriate value for the returned response.
    return_value = lva_service.ListAnalysesResponse()
    # Mock the http request call within the method and fake a response.
    with mock.patch.object(Session, 'request') as req:
        # We need to mock transcode() because providing default values
        # for required fields will fail the real version if the http_options
        # expect actual values for those fields.
        with mock.patch.object(path_template, 'transcode') as transcode:
            # A uri without fields and an empty body will force all the
            # request fields to show up in the query_params.
            pb_request = request_type.pb(request)
            transcode_result = {
                'uri': 'v1/sample_method',
                'method': "get",
                'query_params': pb_request,
            }
            transcode.return_value = transcode_result

            response_value = Response()
            response_value.status_code = 200

            pb_return_value = lva_service.ListAnalysesResponse.pb(return_value)
            json_return_value = json_format.MessageToJson(pb_return_value)

            response_value._content = json_return_value.encode('UTF-8')
            req.return_value = response_value

            response = client.list_analyses(request)

            expected_params = [
            ]
            actual_params = req.call_args.kwargs['params']
            assert expected_params == actual_params


def test_list_analyses_rest_unset_required_fields():
    transport = transports.LiveVideoAnalyticsRestTransport(credentials=ga_credentials.AnonymousCredentials)

    unset_fields = transport.list_analyses._get_unset_required_fields({})
    assert set(unset_fields) == (set(("filter", "orderBy", "pageSize", "pageToken", )) & set(("parent", )))


@pytest.mark.parametrize("null_interceptor", [True, False])
def test_list_analyses_rest_interceptors(null_interceptor):
    transport = transports.LiveVideoAnalyticsRestTransport(
        credentials=ga_credentials.AnonymousCredentials(),
        interceptor=None if null_interceptor else transports.LiveVideoAnalyticsRestInterceptor(),
        )
    client = LiveVideoAnalyticsClient(transport=transport)
    with mock.patch.object(type(client.transport._session), "request") as req, \
         mock.patch.object(path_template, "transcode")  as transcode, \
         mock.patch.object(transports.LiveVideoAnalyticsRestInterceptor, "post_list_analyses") as post, \
         mock.patch.object(transports.LiveVideoAnalyticsRestInterceptor, "pre_list_analyses") as pre:
        pre.assert_not_called()
        post.assert_not_called()
        pb_message = lva_service.ListAnalysesRequest.pb(lva_service.ListAnalysesRequest())
        transcode.return_value = {
            "method": "post",
            "uri": "my_uri",
            "body": pb_message,
            "query_params": pb_message,
        }

        req.return_value = Response()
        req.return_value.status_code = 200
        req.return_value.request = PreparedRequest()
        req.return_value._content = lva_service.ListAnalysesResponse.to_json(lva_service.ListAnalysesResponse())

        request = lva_service.ListAnalysesRequest()
        metadata =[
            ("key", "val"),
            ("cephalopod", "squid"),
        ]
        pre.return_value = request, metadata
        post.return_value = lva_service.ListAnalysesResponse()

        client.list_analyses(request, metadata=[("key", "val"), ("cephalopod", "squid"),])

        pre.assert_called_once()
        post.assert_called_once()


def test_list_analyses_rest_bad_request(transport: str = 'rest', request_type=lva_service.ListAnalysesRequest):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # send a request that will satisfy transcoding
    request_init = {'parent': 'projects/sample1/locations/sample2/clusters/sample3'}
    request = request_type(**request_init)

    # Mock the http request call within the method and fake a BadRequest error.
    with mock.patch.object(Session, 'request') as req, pytest.raises(core_exceptions.BadRequest):
        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 400
        response_value.request = Request()
        req.return_value = response_value
        client.list_analyses(request)


def test_list_analyses_rest_flattened():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport="rest",
    )

    # Mock the http request call within the method and fake a response.
    with mock.patch.object(type(client.transport._session), 'request') as req:
        # Designate an appropriate value for the returned response.
        return_value = lva_service.ListAnalysesResponse()

        # get arguments that satisfy an http rule for this method
        sample_request = {'parent': 'projects/sample1/locations/sample2/clusters/sample3'}

        # get truthy value for each flattened field
        mock_args = dict(
            parent='parent_value',
        )
        mock_args.update(sample_request)

        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 200
        pb_return_value = lva_service.ListAnalysesResponse.pb(return_value)
        json_return_value = json_format.MessageToJson(pb_return_value)
        response_value._content = json_return_value.encode('UTF-8')
        req.return_value = response_value

        client.list_analyses(**mock_args)

        # Establish that the underlying call was made with the expected
        # request object values.
        assert len(req.mock_calls) == 1
        _, args, _ = req.mock_calls[0]
        assert path_template.validate("%s/v1/{parent=projects/*/locations/*/clusters/*}/analyses" % client.transport._host, args[1])


def test_list_analyses_rest_flattened_error(transport: str = 'rest'):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # Attempting to call a method with both a request object and flattened
    # fields is an error.
    with pytest.raises(ValueError):
        client.list_analyses(
            lva_service.ListAnalysesRequest(),
            parent='parent_value',
        )


def test_list_analyses_rest_pager(transport: str = 'rest'):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # Mock the http request call within the method and fake a response.
    with mock.patch.object(Session, 'request') as req:
        # TODO(kbandes): remove this mock unless there's a good reason for it.
        #with mock.patch.object(path_template, 'transcode') as transcode:
        # Set the response as a series of pages
        response = (
            lva_service.ListAnalysesResponse(
                analyses=[
                    lva_resources.Analysis(),
                    lva_resources.Analysis(),
                    lva_resources.Analysis(),
                ],
                next_page_token='abc',
            ),
            lva_service.ListAnalysesResponse(
                analyses=[],
                next_page_token='def',
            ),
            lva_service.ListAnalysesResponse(
                analyses=[
                    lva_resources.Analysis(),
                ],
                next_page_token='ghi',
            ),
            lva_service.ListAnalysesResponse(
                analyses=[
                    lva_resources.Analysis(),
                    lva_resources.Analysis(),
                ],
            ),
        )
        # Two responses for two calls
        response = response + response

        # Wrap the values into proper Response objs
        response = tuple(lva_service.ListAnalysesResponse.to_json(x) for x in response)
        return_values = tuple(Response() for i in response)
        for return_val, response_val in zip(return_values, response):
            return_val._content = response_val.encode('UTF-8')
            return_val.status_code = 200
        req.side_effect = return_values

        sample_request = {'parent': 'projects/sample1/locations/sample2/clusters/sample3'}

        pager = client.list_analyses(request=sample_request)

        results = list(pager)
        assert len(results) == 6
        assert all(isinstance(i, lva_resources.Analysis)
                for i in results)

        pages = list(client.list_analyses(request=sample_request).pages)
        for page_, token in zip(pages, ['abc','def','ghi', '']):
            assert page_.raw_page.next_page_token == token


@pytest.mark.parametrize("request_type", [
    lva_service.GetAnalysisRequest,
    dict,
])
def test_get_analysis_rest(request_type):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport="rest",
    )

    # send a request that will satisfy transcoding
    request_init = {'name': 'projects/sample1/locations/sample2/clusters/sample3/analyses/sample4'}
    request = request_type(**request_init)

    # Mock the http request call within the method and fake a response.
    with mock.patch.object(type(client.transport._session), 'request') as req:
        # Designate an appropriate value for the returned response.
        return_value = lva_resources.Analysis(
              name='name_value',
              disable_event_watch=True,
        )

        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 200
        pb_return_value = lva_resources.Analysis.pb(return_value)
        json_return_value = json_format.MessageToJson(pb_return_value)

        response_value._content = json_return_value.encode('UTF-8')
        req.return_value = response_value
        response = client.get_analysis(request)

    # Establish that the response is the type that we expect.
    assert isinstance(response, lva_resources.Analysis)
    assert response.name == 'name_value'
    assert response.disable_event_watch is True


def test_get_analysis_rest_required_fields(request_type=lva_service.GetAnalysisRequest):
    transport_class = transports.LiveVideoAnalyticsRestTransport

    request_init = {}
    request_init["name"] = ""
    request = request_type(**request_init)
    pb_request = request_type.pb(request)
    jsonified_request = json.loads(json_format.MessageToJson(
        pb_request,
        including_default_value_fields=False,
        use_integers_for_enums=False
    ))

    # verify fields with default values are dropped

    unset_fields = transport_class(credentials=ga_credentials.AnonymousCredentials()).get_analysis._get_unset_required_fields(jsonified_request)
    jsonified_request.update(unset_fields)

    # verify required fields with default values are now present

    jsonified_request["name"] = 'name_value'

    unset_fields = transport_class(credentials=ga_credentials.AnonymousCredentials()).get_analysis._get_unset_required_fields(jsonified_request)
    jsonified_request.update(unset_fields)

    # verify required fields with non-default values are left alone
    assert "name" in jsonified_request
    assert jsonified_request["name"] == 'name_value'

    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport='rest',
    )
    request = request_type(**request_init)

    # Designate an appropriate value for the returned response.
    return_value = lva_resources.Analysis()
    # Mock the http request call within the method and fake a response.
    with mock.patch.object(Session, 'request') as req:
        # We need to mock transcode() because providing default values
        # for required fields will fail the real version if the http_options
        # expect actual values for those fields.
        with mock.patch.object(path_template, 'transcode') as transcode:
            # A uri without fields and an empty body will force all the
            # request fields to show up in the query_params.
            pb_request = request_type.pb(request)
            transcode_result = {
                'uri': 'v1/sample_method',
                'method': "get",
                'query_params': pb_request,
            }
            transcode.return_value = transcode_result

            response_value = Response()
            response_value.status_code = 200

            pb_return_value = lva_resources.Analysis.pb(return_value)
            json_return_value = json_format.MessageToJson(pb_return_value)

            response_value._content = json_return_value.encode('UTF-8')
            req.return_value = response_value

            response = client.get_analysis(request)

            expected_params = [
            ]
            actual_params = req.call_args.kwargs['params']
            assert expected_params == actual_params


def test_get_analysis_rest_unset_required_fields():
    transport = transports.LiveVideoAnalyticsRestTransport(credentials=ga_credentials.AnonymousCredentials)

    unset_fields = transport.get_analysis._get_unset_required_fields({})
    assert set(unset_fields) == (set(()) & set(("name", )))


@pytest.mark.parametrize("null_interceptor", [True, False])
def test_get_analysis_rest_interceptors(null_interceptor):
    transport = transports.LiveVideoAnalyticsRestTransport(
        credentials=ga_credentials.AnonymousCredentials(),
        interceptor=None if null_interceptor else transports.LiveVideoAnalyticsRestInterceptor(),
        )
    client = LiveVideoAnalyticsClient(transport=transport)
    with mock.patch.object(type(client.transport._session), "request") as req, \
         mock.patch.object(path_template, "transcode")  as transcode, \
         mock.patch.object(transports.LiveVideoAnalyticsRestInterceptor, "post_get_analysis") as post, \
         mock.patch.object(transports.LiveVideoAnalyticsRestInterceptor, "pre_get_analysis") as pre:
        pre.assert_not_called()
        post.assert_not_called()
        pb_message = lva_service.GetAnalysisRequest.pb(lva_service.GetAnalysisRequest())
        transcode.return_value = {
            "method": "post",
            "uri": "my_uri",
            "body": pb_message,
            "query_params": pb_message,
        }

        req.return_value = Response()
        req.return_value.status_code = 200
        req.return_value.request = PreparedRequest()
        req.return_value._content = lva_resources.Analysis.to_json(lva_resources.Analysis())

        request = lva_service.GetAnalysisRequest()
        metadata =[
            ("key", "val"),
            ("cephalopod", "squid"),
        ]
        pre.return_value = request, metadata
        post.return_value = lva_resources.Analysis()

        client.get_analysis(request, metadata=[("key", "val"), ("cephalopod", "squid"),])

        pre.assert_called_once()
        post.assert_called_once()


def test_get_analysis_rest_bad_request(transport: str = 'rest', request_type=lva_service.GetAnalysisRequest):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # send a request that will satisfy transcoding
    request_init = {'name': 'projects/sample1/locations/sample2/clusters/sample3/analyses/sample4'}
    request = request_type(**request_init)

    # Mock the http request call within the method and fake a BadRequest error.
    with mock.patch.object(Session, 'request') as req, pytest.raises(core_exceptions.BadRequest):
        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 400
        response_value.request = Request()
        req.return_value = response_value
        client.get_analysis(request)


def test_get_analysis_rest_flattened():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport="rest",
    )

    # Mock the http request call within the method and fake a response.
    with mock.patch.object(type(client.transport._session), 'request') as req:
        # Designate an appropriate value for the returned response.
        return_value = lva_resources.Analysis()

        # get arguments that satisfy an http rule for this method
        sample_request = {'name': 'projects/sample1/locations/sample2/clusters/sample3/analyses/sample4'}

        # get truthy value for each flattened field
        mock_args = dict(
            name='name_value',
        )
        mock_args.update(sample_request)

        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 200
        pb_return_value = lva_resources.Analysis.pb(return_value)
        json_return_value = json_format.MessageToJson(pb_return_value)
        response_value._content = json_return_value.encode('UTF-8')
        req.return_value = response_value

        client.get_analysis(**mock_args)

        # Establish that the underlying call was made with the expected
        # request object values.
        assert len(req.mock_calls) == 1
        _, args, _ = req.mock_calls[0]
        assert path_template.validate("%s/v1/{name=projects/*/locations/*/clusters/*/analyses/*}" % client.transport._host, args[1])


def test_get_analysis_rest_flattened_error(transport: str = 'rest'):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # Attempting to call a method with both a request object and flattened
    # fields is an error.
    with pytest.raises(ValueError):
        client.get_analysis(
            lva_service.GetAnalysisRequest(),
            name='name_value',
        )


def test_get_analysis_rest_error():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport='rest'
    )


@pytest.mark.parametrize("request_type", [
    lva_service.CreateAnalysisRequest,
    dict,
])
def test_create_analysis_rest(request_type):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport="rest",
    )

    # send a request that will satisfy transcoding
    request_init = {'parent': 'projects/sample1/locations/sample2/clusters/sample3'}
    request_init["analysis"] = {'name': 'name_value', 'create_time': {'seconds': 751, 'nanos': 543}, 'update_time': {}, 'labels': {}, 'analysis_definition': {'analyzers': [{'analyzer': 'analyzer_value', 'operator': 'operator_value', 'inputs': [{'input': 'input_value'}], 'attrs': {}, 'debug_options': {'environment_variables': {}}}]}, 'input_streams_mapping': {}, 'output_streams_mapping': {}, 'disable_event_watch': True}
    request = request_type(**request_init)

    # Mock the http request call within the method and fake a response.
    with mock.patch.object(type(client.transport._session), 'request') as req:
        # Designate an appropriate value for the returned response.
        return_value = operations_pb2.Operation(name='operations/spam')

        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 200
        json_return_value = json_format.MessageToJson(return_value)

        response_value._content = json_return_value.encode('UTF-8')
        req.return_value = response_value
        response = client.create_analysis(request)

    # Establish that the response is the type that we expect.
    assert response.operation.name == "operations/spam"


def test_create_analysis_rest_required_fields(request_type=lva_service.CreateAnalysisRequest):
    transport_class = transports.LiveVideoAnalyticsRestTransport

    request_init = {}
    request_init["parent"] = ""
    request_init["analysis_id"] = ""
    request = request_type(**request_init)
    pb_request = request_type.pb(request)
    jsonified_request = json.loads(json_format.MessageToJson(
        pb_request,
        including_default_value_fields=False,
        use_integers_for_enums=False
    ))

    # verify fields with default values are dropped
    assert "analysisId" not in jsonified_request

    unset_fields = transport_class(credentials=ga_credentials.AnonymousCredentials()).create_analysis._get_unset_required_fields(jsonified_request)
    jsonified_request.update(unset_fields)

    # verify required fields with default values are now present
    assert "analysisId" in jsonified_request
    assert jsonified_request["analysisId"] == request_init["analysis_id"]

    jsonified_request["parent"] = 'parent_value'
    jsonified_request["analysisId"] = 'analysis_id_value'

    unset_fields = transport_class(credentials=ga_credentials.AnonymousCredentials()).create_analysis._get_unset_required_fields(jsonified_request)
    # Check that path parameters and body parameters are not mixing in.
    assert not set(unset_fields) - set(("analysis_id", "request_id", ))
    jsonified_request.update(unset_fields)

    # verify required fields with non-default values are left alone
    assert "parent" in jsonified_request
    assert jsonified_request["parent"] == 'parent_value'
    assert "analysisId" in jsonified_request
    assert jsonified_request["analysisId"] == 'analysis_id_value'

    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport='rest',
    )
    request = request_type(**request_init)

    # Designate an appropriate value for the returned response.
    return_value = operations_pb2.Operation(name='operations/spam')
    # Mock the http request call within the method and fake a response.
    with mock.patch.object(Session, 'request') as req:
        # We need to mock transcode() because providing default values
        # for required fields will fail the real version if the http_options
        # expect actual values for those fields.
        with mock.patch.object(path_template, 'transcode') as transcode:
            # A uri without fields and an empty body will force all the
            # request fields to show up in the query_params.
            pb_request = request_type.pb(request)
            transcode_result = {
                'uri': 'v1/sample_method',
                'method': "post",
                'query_params': pb_request,
            }
            transcode_result['body'] = pb_request
            transcode.return_value = transcode_result

            response_value = Response()
            response_value.status_code = 200
            json_return_value = json_format.MessageToJson(return_value)

            response_value._content = json_return_value.encode('UTF-8')
            req.return_value = response_value

            response = client.create_analysis(request)

            expected_params = [
                (
                    "analysisId",
                    "",
                ),
            ]
            actual_params = req.call_args.kwargs['params']
            assert expected_params == actual_params


def test_create_analysis_rest_unset_required_fields():
    transport = transports.LiveVideoAnalyticsRestTransport(credentials=ga_credentials.AnonymousCredentials)

    unset_fields = transport.create_analysis._get_unset_required_fields({})
    assert set(unset_fields) == (set(("analysisId", "requestId", )) & set(("parent", "analysisId", "analysis", )))


@pytest.mark.parametrize("null_interceptor", [True, False])
def test_create_analysis_rest_interceptors(null_interceptor):
    transport = transports.LiveVideoAnalyticsRestTransport(
        credentials=ga_credentials.AnonymousCredentials(),
        interceptor=None if null_interceptor else transports.LiveVideoAnalyticsRestInterceptor(),
        )
    client = LiveVideoAnalyticsClient(transport=transport)
    with mock.patch.object(type(client.transport._session), "request") as req, \
         mock.patch.object(path_template, "transcode")  as transcode, \
         mock.patch.object(operation.Operation, "_set_result_from_operation"), \
         mock.patch.object(transports.LiveVideoAnalyticsRestInterceptor, "post_create_analysis") as post, \
         mock.patch.object(transports.LiveVideoAnalyticsRestInterceptor, "pre_create_analysis") as pre:
        pre.assert_not_called()
        post.assert_not_called()
        pb_message = lva_service.CreateAnalysisRequest.pb(lva_service.CreateAnalysisRequest())
        transcode.return_value = {
            "method": "post",
            "uri": "my_uri",
            "body": pb_message,
            "query_params": pb_message,
        }

        req.return_value = Response()
        req.return_value.status_code = 200
        req.return_value.request = PreparedRequest()
        req.return_value._content = json_format.MessageToJson(operations_pb2.Operation())

        request = lva_service.CreateAnalysisRequest()
        metadata =[
            ("key", "val"),
            ("cephalopod", "squid"),
        ]
        pre.return_value = request, metadata
        post.return_value = operations_pb2.Operation()

        client.create_analysis(request, metadata=[("key", "val"), ("cephalopod", "squid"),])

        pre.assert_called_once()
        post.assert_called_once()


def test_create_analysis_rest_bad_request(transport: str = 'rest', request_type=lva_service.CreateAnalysisRequest):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # send a request that will satisfy transcoding
    request_init = {'parent': 'projects/sample1/locations/sample2/clusters/sample3'}
    request_init["analysis"] = {'name': 'name_value', 'create_time': {'seconds': 751, 'nanos': 543}, 'update_time': {}, 'labels': {}, 'analysis_definition': {'analyzers': [{'analyzer': 'analyzer_value', 'operator': 'operator_value', 'inputs': [{'input': 'input_value'}], 'attrs': {}, 'debug_options': {'environment_variables': {}}}]}, 'input_streams_mapping': {}, 'output_streams_mapping': {}, 'disable_event_watch': True}
    request = request_type(**request_init)

    # Mock the http request call within the method and fake a BadRequest error.
    with mock.patch.object(Session, 'request') as req, pytest.raises(core_exceptions.BadRequest):
        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 400
        response_value.request = Request()
        req.return_value = response_value
        client.create_analysis(request)


def test_create_analysis_rest_flattened():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport="rest",
    )

    # Mock the http request call within the method and fake a response.
    with mock.patch.object(type(client.transport._session), 'request') as req:
        # Designate an appropriate value for the returned response.
        return_value = operations_pb2.Operation(name='operations/spam')

        # get arguments that satisfy an http rule for this method
        sample_request = {'parent': 'projects/sample1/locations/sample2/clusters/sample3'}

        # get truthy value for each flattened field
        mock_args = dict(
            parent='parent_value',
            analysis=lva_resources.Analysis(name='name_value'),
            analysis_id='analysis_id_value',
        )
        mock_args.update(sample_request)

        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 200
        json_return_value = json_format.MessageToJson(return_value)
        response_value._content = json_return_value.encode('UTF-8')
        req.return_value = response_value

        client.create_analysis(**mock_args)

        # Establish that the underlying call was made with the expected
        # request object values.
        assert len(req.mock_calls) == 1
        _, args, _ = req.mock_calls[0]
        assert path_template.validate("%s/v1/{parent=projects/*/locations/*/clusters/*}/analyses" % client.transport._host, args[1])


def test_create_analysis_rest_flattened_error(transport: str = 'rest'):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # Attempting to call a method with both a request object and flattened
    # fields is an error.
    with pytest.raises(ValueError):
        client.create_analysis(
            lva_service.CreateAnalysisRequest(),
            parent='parent_value',
            analysis=lva_resources.Analysis(name='name_value'),
            analysis_id='analysis_id_value',
        )


def test_create_analysis_rest_error():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport='rest'
    )


@pytest.mark.parametrize("request_type", [
    lva_service.UpdateAnalysisRequest,
    dict,
])
def test_update_analysis_rest(request_type):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport="rest",
    )

    # send a request that will satisfy transcoding
    request_init = {'analysis': {'name': 'projects/sample1/locations/sample2/clusters/sample3/analyses/sample4'}}
    request_init["analysis"] = {'name': 'projects/sample1/locations/sample2/clusters/sample3/analyses/sample4', 'create_time': {'seconds': 751, 'nanos': 543}, 'update_time': {}, 'labels': {}, 'analysis_definition': {'analyzers': [{'analyzer': 'analyzer_value', 'operator': 'operator_value', 'inputs': [{'input': 'input_value'}], 'attrs': {}, 'debug_options': {'environment_variables': {}}}]}, 'input_streams_mapping': {}, 'output_streams_mapping': {}, 'disable_event_watch': True}
    request = request_type(**request_init)

    # Mock the http request call within the method and fake a response.
    with mock.patch.object(type(client.transport._session), 'request') as req:
        # Designate an appropriate value for the returned response.
        return_value = operations_pb2.Operation(name='operations/spam')

        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 200
        json_return_value = json_format.MessageToJson(return_value)

        response_value._content = json_return_value.encode('UTF-8')
        req.return_value = response_value
        response = client.update_analysis(request)

    # Establish that the response is the type that we expect.
    assert response.operation.name == "operations/spam"


def test_update_analysis_rest_required_fields(request_type=lva_service.UpdateAnalysisRequest):
    transport_class = transports.LiveVideoAnalyticsRestTransport

    request_init = {}
    request = request_type(**request_init)
    pb_request = request_type.pb(request)
    jsonified_request = json.loads(json_format.MessageToJson(
        pb_request,
        including_default_value_fields=False,
        use_integers_for_enums=False
    ))

    # verify fields with default values are dropped

    unset_fields = transport_class(credentials=ga_credentials.AnonymousCredentials()).update_analysis._get_unset_required_fields(jsonified_request)
    jsonified_request.update(unset_fields)

    # verify required fields with default values are now present

    unset_fields = transport_class(credentials=ga_credentials.AnonymousCredentials()).update_analysis._get_unset_required_fields(jsonified_request)
    # Check that path parameters and body parameters are not mixing in.
    assert not set(unset_fields) - set(("request_id", "update_mask", ))
    jsonified_request.update(unset_fields)

    # verify required fields with non-default values are left alone

    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport='rest',
    )
    request = request_type(**request_init)

    # Designate an appropriate value for the returned response.
    return_value = operations_pb2.Operation(name='operations/spam')
    # Mock the http request call within the method and fake a response.
    with mock.patch.object(Session, 'request') as req:
        # We need to mock transcode() because providing default values
        # for required fields will fail the real version if the http_options
        # expect actual values for those fields.
        with mock.patch.object(path_template, 'transcode') as transcode:
            # A uri without fields and an empty body will force all the
            # request fields to show up in the query_params.
            pb_request = request_type.pb(request)
            transcode_result = {
                'uri': 'v1/sample_method',
                'method': "patch",
                'query_params': pb_request,
            }
            transcode_result['body'] = pb_request
            transcode.return_value = transcode_result

            response_value = Response()
            response_value.status_code = 200
            json_return_value = json_format.MessageToJson(return_value)

            response_value._content = json_return_value.encode('UTF-8')
            req.return_value = response_value

            response = client.update_analysis(request)

            expected_params = [
            ]
            actual_params = req.call_args.kwargs['params']
            assert expected_params == actual_params


def test_update_analysis_rest_unset_required_fields():
    transport = transports.LiveVideoAnalyticsRestTransport(credentials=ga_credentials.AnonymousCredentials)

    unset_fields = transport.update_analysis._get_unset_required_fields({})
    assert set(unset_fields) == (set(("requestId", "updateMask", )) & set(("updateMask", "analysis", )))


@pytest.mark.parametrize("null_interceptor", [True, False])
def test_update_analysis_rest_interceptors(null_interceptor):
    transport = transports.LiveVideoAnalyticsRestTransport(
        credentials=ga_credentials.AnonymousCredentials(),
        interceptor=None if null_interceptor else transports.LiveVideoAnalyticsRestInterceptor(),
        )
    client = LiveVideoAnalyticsClient(transport=transport)
    with mock.patch.object(type(client.transport._session), "request") as req, \
         mock.patch.object(path_template, "transcode")  as transcode, \
         mock.patch.object(operation.Operation, "_set_result_from_operation"), \
         mock.patch.object(transports.LiveVideoAnalyticsRestInterceptor, "post_update_analysis") as post, \
         mock.patch.object(transports.LiveVideoAnalyticsRestInterceptor, "pre_update_analysis") as pre:
        pre.assert_not_called()
        post.assert_not_called()
        pb_message = lva_service.UpdateAnalysisRequest.pb(lva_service.UpdateAnalysisRequest())
        transcode.return_value = {
            "method": "post",
            "uri": "my_uri",
            "body": pb_message,
            "query_params": pb_message,
        }

        req.return_value = Response()
        req.return_value.status_code = 200
        req.return_value.request = PreparedRequest()
        req.return_value._content = json_format.MessageToJson(operations_pb2.Operation())

        request = lva_service.UpdateAnalysisRequest()
        metadata =[
            ("key", "val"),
            ("cephalopod", "squid"),
        ]
        pre.return_value = request, metadata
        post.return_value = operations_pb2.Operation()

        client.update_analysis(request, metadata=[("key", "val"), ("cephalopod", "squid"),])

        pre.assert_called_once()
        post.assert_called_once()


def test_update_analysis_rest_bad_request(transport: str = 'rest', request_type=lva_service.UpdateAnalysisRequest):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # send a request that will satisfy transcoding
    request_init = {'analysis': {'name': 'projects/sample1/locations/sample2/clusters/sample3/analyses/sample4'}}
    request_init["analysis"] = {'name': 'projects/sample1/locations/sample2/clusters/sample3/analyses/sample4', 'create_time': {'seconds': 751, 'nanos': 543}, 'update_time': {}, 'labels': {}, 'analysis_definition': {'analyzers': [{'analyzer': 'analyzer_value', 'operator': 'operator_value', 'inputs': [{'input': 'input_value'}], 'attrs': {}, 'debug_options': {'environment_variables': {}}}]}, 'input_streams_mapping': {}, 'output_streams_mapping': {}, 'disable_event_watch': True}
    request = request_type(**request_init)

    # Mock the http request call within the method and fake a BadRequest error.
    with mock.patch.object(Session, 'request') as req, pytest.raises(core_exceptions.BadRequest):
        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 400
        response_value.request = Request()
        req.return_value = response_value
        client.update_analysis(request)


def test_update_analysis_rest_flattened():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport="rest",
    )

    # Mock the http request call within the method and fake a response.
    with mock.patch.object(type(client.transport._session), 'request') as req:
        # Designate an appropriate value for the returned response.
        return_value = operations_pb2.Operation(name='operations/spam')

        # get arguments that satisfy an http rule for this method
        sample_request = {'analysis': {'name': 'projects/sample1/locations/sample2/clusters/sample3/analyses/sample4'}}

        # get truthy value for each flattened field
        mock_args = dict(
            analysis=lva_resources.Analysis(name='name_value'),
            update_mask=field_mask_pb2.FieldMask(paths=['paths_value']),
        )
        mock_args.update(sample_request)

        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 200
        json_return_value = json_format.MessageToJson(return_value)
        response_value._content = json_return_value.encode('UTF-8')
        req.return_value = response_value

        client.update_analysis(**mock_args)

        # Establish that the underlying call was made with the expected
        # request object values.
        assert len(req.mock_calls) == 1
        _, args, _ = req.mock_calls[0]
        assert path_template.validate("%s/v1/{analysis.name=projects/*/locations/*/clusters/*/analyses/*}" % client.transport._host, args[1])


def test_update_analysis_rest_flattened_error(transport: str = 'rest'):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # Attempting to call a method with both a request object and flattened
    # fields is an error.
    with pytest.raises(ValueError):
        client.update_analysis(
            lva_service.UpdateAnalysisRequest(),
            analysis=lva_resources.Analysis(name='name_value'),
            update_mask=field_mask_pb2.FieldMask(paths=['paths_value']),
        )


def test_update_analysis_rest_error():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport='rest'
    )


@pytest.mark.parametrize("request_type", [
    lva_service.DeleteAnalysisRequest,
    dict,
])
def test_delete_analysis_rest(request_type):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport="rest",
    )

    # send a request that will satisfy transcoding
    request_init = {'name': 'projects/sample1/locations/sample2/clusters/sample3/analyses/sample4'}
    request = request_type(**request_init)

    # Mock the http request call within the method and fake a response.
    with mock.patch.object(type(client.transport._session), 'request') as req:
        # Designate an appropriate value for the returned response.
        return_value = operations_pb2.Operation(name='operations/spam')

        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 200
        json_return_value = json_format.MessageToJson(return_value)

        response_value._content = json_return_value.encode('UTF-8')
        req.return_value = response_value
        response = client.delete_analysis(request)

    # Establish that the response is the type that we expect.
    assert response.operation.name == "operations/spam"


def test_delete_analysis_rest_required_fields(request_type=lva_service.DeleteAnalysisRequest):
    transport_class = transports.LiveVideoAnalyticsRestTransport

    request_init = {}
    request_init["name"] = ""
    request = request_type(**request_init)
    pb_request = request_type.pb(request)
    jsonified_request = json.loads(json_format.MessageToJson(
        pb_request,
        including_default_value_fields=False,
        use_integers_for_enums=False
    ))

    # verify fields with default values are dropped

    unset_fields = transport_class(credentials=ga_credentials.AnonymousCredentials()).delete_analysis._get_unset_required_fields(jsonified_request)
    jsonified_request.update(unset_fields)

    # verify required fields with default values are now present

    jsonified_request["name"] = 'name_value'

    unset_fields = transport_class(credentials=ga_credentials.AnonymousCredentials()).delete_analysis._get_unset_required_fields(jsonified_request)
    # Check that path parameters and body parameters are not mixing in.
    assert not set(unset_fields) - set(("request_id", ))
    jsonified_request.update(unset_fields)

    # verify required fields with non-default values are left alone
    assert "name" in jsonified_request
    assert jsonified_request["name"] == 'name_value'

    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport='rest',
    )
    request = request_type(**request_init)

    # Designate an appropriate value for the returned response.
    return_value = operations_pb2.Operation(name='operations/spam')
    # Mock the http request call within the method and fake a response.
    with mock.patch.object(Session, 'request') as req:
        # We need to mock transcode() because providing default values
        # for required fields will fail the real version if the http_options
        # expect actual values for those fields.
        with mock.patch.object(path_template, 'transcode') as transcode:
            # A uri without fields and an empty body will force all the
            # request fields to show up in the query_params.
            pb_request = request_type.pb(request)
            transcode_result = {
                'uri': 'v1/sample_method',
                'method': "delete",
                'query_params': pb_request,
            }
            transcode.return_value = transcode_result

            response_value = Response()
            response_value.status_code = 200
            json_return_value = json_format.MessageToJson(return_value)

            response_value._content = json_return_value.encode('UTF-8')
            req.return_value = response_value

            response = client.delete_analysis(request)

            expected_params = [
            ]
            actual_params = req.call_args.kwargs['params']
            assert expected_params == actual_params


def test_delete_analysis_rest_unset_required_fields():
    transport = transports.LiveVideoAnalyticsRestTransport(credentials=ga_credentials.AnonymousCredentials)

    unset_fields = transport.delete_analysis._get_unset_required_fields({})
    assert set(unset_fields) == (set(("requestId", )) & set(("name", )))


@pytest.mark.parametrize("null_interceptor", [True, False])
def test_delete_analysis_rest_interceptors(null_interceptor):
    transport = transports.LiveVideoAnalyticsRestTransport(
        credentials=ga_credentials.AnonymousCredentials(),
        interceptor=None if null_interceptor else transports.LiveVideoAnalyticsRestInterceptor(),
        )
    client = LiveVideoAnalyticsClient(transport=transport)
    with mock.patch.object(type(client.transport._session), "request") as req, \
         mock.patch.object(path_template, "transcode")  as transcode, \
         mock.patch.object(operation.Operation, "_set_result_from_operation"), \
         mock.patch.object(transports.LiveVideoAnalyticsRestInterceptor, "post_delete_analysis") as post, \
         mock.patch.object(transports.LiveVideoAnalyticsRestInterceptor, "pre_delete_analysis") as pre:
        pre.assert_not_called()
        post.assert_not_called()
        pb_message = lva_service.DeleteAnalysisRequest.pb(lva_service.DeleteAnalysisRequest())
        transcode.return_value = {
            "method": "post",
            "uri": "my_uri",
            "body": pb_message,
            "query_params": pb_message,
        }

        req.return_value = Response()
        req.return_value.status_code = 200
        req.return_value.request = PreparedRequest()
        req.return_value._content = json_format.MessageToJson(operations_pb2.Operation())

        request = lva_service.DeleteAnalysisRequest()
        metadata =[
            ("key", "val"),
            ("cephalopod", "squid"),
        ]
        pre.return_value = request, metadata
        post.return_value = operations_pb2.Operation()

        client.delete_analysis(request, metadata=[("key", "val"), ("cephalopod", "squid"),])

        pre.assert_called_once()
        post.assert_called_once()


def test_delete_analysis_rest_bad_request(transport: str = 'rest', request_type=lva_service.DeleteAnalysisRequest):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # send a request that will satisfy transcoding
    request_init = {'name': 'projects/sample1/locations/sample2/clusters/sample3/analyses/sample4'}
    request = request_type(**request_init)

    # Mock the http request call within the method and fake a BadRequest error.
    with mock.patch.object(Session, 'request') as req, pytest.raises(core_exceptions.BadRequest):
        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 400
        response_value.request = Request()
        req.return_value = response_value
        client.delete_analysis(request)


def test_delete_analysis_rest_flattened():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport="rest",
    )

    # Mock the http request call within the method and fake a response.
    with mock.patch.object(type(client.transport._session), 'request') as req:
        # Designate an appropriate value for the returned response.
        return_value = operations_pb2.Operation(name='operations/spam')

        # get arguments that satisfy an http rule for this method
        sample_request = {'name': 'projects/sample1/locations/sample2/clusters/sample3/analyses/sample4'}

        # get truthy value for each flattened field
        mock_args = dict(
            name='name_value',
        )
        mock_args.update(sample_request)

        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 200
        json_return_value = json_format.MessageToJson(return_value)
        response_value._content = json_return_value.encode('UTF-8')
        req.return_value = response_value

        client.delete_analysis(**mock_args)

        # Establish that the underlying call was made with the expected
        # request object values.
        assert len(req.mock_calls) == 1
        _, args, _ = req.mock_calls[0]
        assert path_template.validate("%s/v1/{name=projects/*/locations/*/clusters/*/analyses/*}" % client.transport._host, args[1])


def test_delete_analysis_rest_flattened_error(transport: str = 'rest'):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # Attempting to call a method with both a request object and flattened
    # fields is an error.
    with pytest.raises(ValueError):
        client.delete_analysis(
            lva_service.DeleteAnalysisRequest(),
            name='name_value',
        )


def test_delete_analysis_rest_error():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport='rest'
    )


@pytest.mark.parametrize("request_type", [
    lva_service.ListProcessesRequest,
    dict,
])
def test_list_processes_rest(request_type):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport="rest",
    )

    # send a request that will satisfy transcoding
    request_init = {'parent': 'projects/sample1/locations/sample2/clusters/sample3'}
    request = request_type(**request_init)

    # Mock the http request call within the method and fake a response.
    with mock.patch.object(type(client.transport._session), 'request') as req:
        # Designate an appropriate value for the returned response.
        return_value = lva_service.ListProcessesResponse(
              next_page_token='next_page_token_value',
              unreachable=['unreachable_value'],
        )

        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 200
        pb_return_value = lva_service.ListProcessesResponse.pb(return_value)
        json_return_value = json_format.MessageToJson(pb_return_value)

        response_value._content = json_return_value.encode('UTF-8')
        req.return_value = response_value
        response = client.list_processes(request)

    # Establish that the response is the type that we expect.
    assert isinstance(response, pagers.ListProcessesPager)
    assert response.next_page_token == 'next_page_token_value'
    assert response.unreachable == ['unreachable_value']


def test_list_processes_rest_required_fields(request_type=lva_service.ListProcessesRequest):
    transport_class = transports.LiveVideoAnalyticsRestTransport

    request_init = {}
    request_init["parent"] = ""
    request = request_type(**request_init)
    pb_request = request_type.pb(request)
    jsonified_request = json.loads(json_format.MessageToJson(
        pb_request,
        including_default_value_fields=False,
        use_integers_for_enums=False
    ))

    # verify fields with default values are dropped

    unset_fields = transport_class(credentials=ga_credentials.AnonymousCredentials()).list_processes._get_unset_required_fields(jsonified_request)
    jsonified_request.update(unset_fields)

    # verify required fields with default values are now present

    jsonified_request["parent"] = 'parent_value'

    unset_fields = transport_class(credentials=ga_credentials.AnonymousCredentials()).list_processes._get_unset_required_fields(jsonified_request)
    # Check that path parameters and body parameters are not mixing in.
    assert not set(unset_fields) - set(("filter", "order_by", "page_size", "page_token", ))
    jsonified_request.update(unset_fields)

    # verify required fields with non-default values are left alone
    assert "parent" in jsonified_request
    assert jsonified_request["parent"] == 'parent_value'

    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport='rest',
    )
    request = request_type(**request_init)

    # Designate an appropriate value for the returned response.
    return_value = lva_service.ListProcessesResponse()
    # Mock the http request call within the method and fake a response.
    with mock.patch.object(Session, 'request') as req:
        # We need to mock transcode() because providing default values
        # for required fields will fail the real version if the http_options
        # expect actual values for those fields.
        with mock.patch.object(path_template, 'transcode') as transcode:
            # A uri without fields and an empty body will force all the
            # request fields to show up in the query_params.
            pb_request = request_type.pb(request)
            transcode_result = {
                'uri': 'v1/sample_method',
                'method': "get",
                'query_params': pb_request,
            }
            transcode.return_value = transcode_result

            response_value = Response()
            response_value.status_code = 200

            pb_return_value = lva_service.ListProcessesResponse.pb(return_value)
            json_return_value = json_format.MessageToJson(pb_return_value)

            response_value._content = json_return_value.encode('UTF-8')
            req.return_value = response_value

            response = client.list_processes(request)

            expected_params = [
            ]
            actual_params = req.call_args.kwargs['params']
            assert expected_params == actual_params


def test_list_processes_rest_unset_required_fields():
    transport = transports.LiveVideoAnalyticsRestTransport(credentials=ga_credentials.AnonymousCredentials)

    unset_fields = transport.list_processes._get_unset_required_fields({})
    assert set(unset_fields) == (set(("filter", "orderBy", "pageSize", "pageToken", )) & set(("parent", )))


@pytest.mark.parametrize("null_interceptor", [True, False])
def test_list_processes_rest_interceptors(null_interceptor):
    transport = transports.LiveVideoAnalyticsRestTransport(
        credentials=ga_credentials.AnonymousCredentials(),
        interceptor=None if null_interceptor else transports.LiveVideoAnalyticsRestInterceptor(),
        )
    client = LiveVideoAnalyticsClient(transport=transport)
    with mock.patch.object(type(client.transport._session), "request") as req, \
         mock.patch.object(path_template, "transcode")  as transcode, \
         mock.patch.object(transports.LiveVideoAnalyticsRestInterceptor, "post_list_processes") as post, \
         mock.patch.object(transports.LiveVideoAnalyticsRestInterceptor, "pre_list_processes") as pre:
        pre.assert_not_called()
        post.assert_not_called()
        pb_message = lva_service.ListProcessesRequest.pb(lva_service.ListProcessesRequest())
        transcode.return_value = {
            "method": "post",
            "uri": "my_uri",
            "body": pb_message,
            "query_params": pb_message,
        }

        req.return_value = Response()
        req.return_value.status_code = 200
        req.return_value.request = PreparedRequest()
        req.return_value._content = lva_service.ListProcessesResponse.to_json(lva_service.ListProcessesResponse())

        request = lva_service.ListProcessesRequest()
        metadata =[
            ("key", "val"),
            ("cephalopod", "squid"),
        ]
        pre.return_value = request, metadata
        post.return_value = lva_service.ListProcessesResponse()

        client.list_processes(request, metadata=[("key", "val"), ("cephalopod", "squid"),])

        pre.assert_called_once()
        post.assert_called_once()


def test_list_processes_rest_bad_request(transport: str = 'rest', request_type=lva_service.ListProcessesRequest):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # send a request that will satisfy transcoding
    request_init = {'parent': 'projects/sample1/locations/sample2/clusters/sample3'}
    request = request_type(**request_init)

    # Mock the http request call within the method and fake a BadRequest error.
    with mock.patch.object(Session, 'request') as req, pytest.raises(core_exceptions.BadRequest):
        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 400
        response_value.request = Request()
        req.return_value = response_value
        client.list_processes(request)


def test_list_processes_rest_flattened():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport="rest",
    )

    # Mock the http request call within the method and fake a response.
    with mock.patch.object(type(client.transport._session), 'request') as req:
        # Designate an appropriate value for the returned response.
        return_value = lva_service.ListProcessesResponse()

        # get arguments that satisfy an http rule for this method
        sample_request = {'parent': 'projects/sample1/locations/sample2/clusters/sample3'}

        # get truthy value for each flattened field
        mock_args = dict(
            parent='parent_value',
        )
        mock_args.update(sample_request)

        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 200
        pb_return_value = lva_service.ListProcessesResponse.pb(return_value)
        json_return_value = json_format.MessageToJson(pb_return_value)
        response_value._content = json_return_value.encode('UTF-8')
        req.return_value = response_value

        client.list_processes(**mock_args)

        # Establish that the underlying call was made with the expected
        # request object values.
        assert len(req.mock_calls) == 1
        _, args, _ = req.mock_calls[0]
        assert path_template.validate("%s/v1/{parent=projects/*/locations/*/clusters/*}/processes" % client.transport._host, args[1])


def test_list_processes_rest_flattened_error(transport: str = 'rest'):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # Attempting to call a method with both a request object and flattened
    # fields is an error.
    with pytest.raises(ValueError):
        client.list_processes(
            lva_service.ListProcessesRequest(),
            parent='parent_value',
        )


def test_list_processes_rest_pager(transport: str = 'rest'):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # Mock the http request call within the method and fake a response.
    with mock.patch.object(Session, 'request') as req:
        # TODO(kbandes): remove this mock unless there's a good reason for it.
        #with mock.patch.object(path_template, 'transcode') as transcode:
        # Set the response as a series of pages
        response = (
            lva_service.ListProcessesResponse(
                processes=[
                    lva_resources.Process(),
                    lva_resources.Process(),
                    lva_resources.Process(),
                ],
                next_page_token='abc',
            ),
            lva_service.ListProcessesResponse(
                processes=[],
                next_page_token='def',
            ),
            lva_service.ListProcessesResponse(
                processes=[
                    lva_resources.Process(),
                ],
                next_page_token='ghi',
            ),
            lva_service.ListProcessesResponse(
                processes=[
                    lva_resources.Process(),
                    lva_resources.Process(),
                ],
            ),
        )
        # Two responses for two calls
        response = response + response

        # Wrap the values into proper Response objs
        response = tuple(lva_service.ListProcessesResponse.to_json(x) for x in response)
        return_values = tuple(Response() for i in response)
        for return_val, response_val in zip(return_values, response):
            return_val._content = response_val.encode('UTF-8')
            return_val.status_code = 200
        req.side_effect = return_values

        sample_request = {'parent': 'projects/sample1/locations/sample2/clusters/sample3'}

        pager = client.list_processes(request=sample_request)

        results = list(pager)
        assert len(results) == 6
        assert all(isinstance(i, lva_resources.Process)
                for i in results)

        pages = list(client.list_processes(request=sample_request).pages)
        for page_, token in zip(pages, ['abc','def','ghi', '']):
            assert page_.raw_page.next_page_token == token


@pytest.mark.parametrize("request_type", [
    lva_service.GetProcessRequest,
    dict,
])
def test_get_process_rest(request_type):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport="rest",
    )

    # send a request that will satisfy transcoding
    request_init = {'name': 'projects/sample1/locations/sample2/clusters/sample3/processes/sample4'}
    request = request_type(**request_init)

    # Mock the http request call within the method and fake a response.
    with mock.patch.object(type(client.transport._session), 'request') as req:
        # Designate an appropriate value for the returned response.
        return_value = lva_resources.Process(
              name='name_value',
              analysis='analysis_value',
              attribute_overrides=['attribute_overrides_value'],
              run_mode=lva.RunMode.LIVE,
              event_id='event_id_value',
              batch_id='batch_id_value',
              retry_count=1214,
        )

        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 200
        pb_return_value = lva_resources.Process.pb(return_value)
        json_return_value = json_format.MessageToJson(pb_return_value)

        response_value._content = json_return_value.encode('UTF-8')
        req.return_value = response_value
        response = client.get_process(request)

    # Establish that the response is the type that we expect.
    assert isinstance(response, lva_resources.Process)
    assert response.name == 'name_value'
    assert response.analysis == 'analysis_value'
    assert response.attribute_overrides == ['attribute_overrides_value']
    assert response.run_mode == lva.RunMode.LIVE
    assert response.event_id == 'event_id_value'
    assert response.batch_id == 'batch_id_value'
    assert response.retry_count == 1214


def test_get_process_rest_required_fields(request_type=lva_service.GetProcessRequest):
    transport_class = transports.LiveVideoAnalyticsRestTransport

    request_init = {}
    request_init["name"] = ""
    request = request_type(**request_init)
    pb_request = request_type.pb(request)
    jsonified_request = json.loads(json_format.MessageToJson(
        pb_request,
        including_default_value_fields=False,
        use_integers_for_enums=False
    ))

    # verify fields with default values are dropped

    unset_fields = transport_class(credentials=ga_credentials.AnonymousCredentials()).get_process._get_unset_required_fields(jsonified_request)
    jsonified_request.update(unset_fields)

    # verify required fields with default values are now present

    jsonified_request["name"] = 'name_value'

    unset_fields = transport_class(credentials=ga_credentials.AnonymousCredentials()).get_process._get_unset_required_fields(jsonified_request)
    jsonified_request.update(unset_fields)

    # verify required fields with non-default values are left alone
    assert "name" in jsonified_request
    assert jsonified_request["name"] == 'name_value'

    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport='rest',
    )
    request = request_type(**request_init)

    # Designate an appropriate value for the returned response.
    return_value = lva_resources.Process()
    # Mock the http request call within the method and fake a response.
    with mock.patch.object(Session, 'request') as req:
        # We need to mock transcode() because providing default values
        # for required fields will fail the real version if the http_options
        # expect actual values for those fields.
        with mock.patch.object(path_template, 'transcode') as transcode:
            # A uri without fields and an empty body will force all the
            # request fields to show up in the query_params.
            pb_request = request_type.pb(request)
            transcode_result = {
                'uri': 'v1/sample_method',
                'method': "get",
                'query_params': pb_request,
            }
            transcode.return_value = transcode_result

            response_value = Response()
            response_value.status_code = 200

            pb_return_value = lva_resources.Process.pb(return_value)
            json_return_value = json_format.MessageToJson(pb_return_value)

            response_value._content = json_return_value.encode('UTF-8')
            req.return_value = response_value

            response = client.get_process(request)

            expected_params = [
            ]
            actual_params = req.call_args.kwargs['params']
            assert expected_params == actual_params


def test_get_process_rest_unset_required_fields():
    transport = transports.LiveVideoAnalyticsRestTransport(credentials=ga_credentials.AnonymousCredentials)

    unset_fields = transport.get_process._get_unset_required_fields({})
    assert set(unset_fields) == (set(()) & set(("name", )))


@pytest.mark.parametrize("null_interceptor", [True, False])
def test_get_process_rest_interceptors(null_interceptor):
    transport = transports.LiveVideoAnalyticsRestTransport(
        credentials=ga_credentials.AnonymousCredentials(),
        interceptor=None if null_interceptor else transports.LiveVideoAnalyticsRestInterceptor(),
        )
    client = LiveVideoAnalyticsClient(transport=transport)
    with mock.patch.object(type(client.transport._session), "request") as req, \
         mock.patch.object(path_template, "transcode")  as transcode, \
         mock.patch.object(transports.LiveVideoAnalyticsRestInterceptor, "post_get_process") as post, \
         mock.patch.object(transports.LiveVideoAnalyticsRestInterceptor, "pre_get_process") as pre:
        pre.assert_not_called()
        post.assert_not_called()
        pb_message = lva_service.GetProcessRequest.pb(lva_service.GetProcessRequest())
        transcode.return_value = {
            "method": "post",
            "uri": "my_uri",
            "body": pb_message,
            "query_params": pb_message,
        }

        req.return_value = Response()
        req.return_value.status_code = 200
        req.return_value.request = PreparedRequest()
        req.return_value._content = lva_resources.Process.to_json(lva_resources.Process())

        request = lva_service.GetProcessRequest()
        metadata =[
            ("key", "val"),
            ("cephalopod", "squid"),
        ]
        pre.return_value = request, metadata
        post.return_value = lva_resources.Process()

        client.get_process(request, metadata=[("key", "val"), ("cephalopod", "squid"),])

        pre.assert_called_once()
        post.assert_called_once()


def test_get_process_rest_bad_request(transport: str = 'rest', request_type=lva_service.GetProcessRequest):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # send a request that will satisfy transcoding
    request_init = {'name': 'projects/sample1/locations/sample2/clusters/sample3/processes/sample4'}
    request = request_type(**request_init)

    # Mock the http request call within the method and fake a BadRequest error.
    with mock.patch.object(Session, 'request') as req, pytest.raises(core_exceptions.BadRequest):
        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 400
        response_value.request = Request()
        req.return_value = response_value
        client.get_process(request)


def test_get_process_rest_flattened():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport="rest",
    )

    # Mock the http request call within the method and fake a response.
    with mock.patch.object(type(client.transport._session), 'request') as req:
        # Designate an appropriate value for the returned response.
        return_value = lva_resources.Process()

        # get arguments that satisfy an http rule for this method
        sample_request = {'name': 'projects/sample1/locations/sample2/clusters/sample3/processes/sample4'}

        # get truthy value for each flattened field
        mock_args = dict(
            name='name_value',
        )
        mock_args.update(sample_request)

        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 200
        pb_return_value = lva_resources.Process.pb(return_value)
        json_return_value = json_format.MessageToJson(pb_return_value)
        response_value._content = json_return_value.encode('UTF-8')
        req.return_value = response_value

        client.get_process(**mock_args)

        # Establish that the underlying call was made with the expected
        # request object values.
        assert len(req.mock_calls) == 1
        _, args, _ = req.mock_calls[0]
        assert path_template.validate("%s/v1/{name=projects/*/locations/*/clusters/*/processes/*}" % client.transport._host, args[1])


def test_get_process_rest_flattened_error(transport: str = 'rest'):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # Attempting to call a method with both a request object and flattened
    # fields is an error.
    with pytest.raises(ValueError):
        client.get_process(
            lva_service.GetProcessRequest(),
            name='name_value',
        )


def test_get_process_rest_error():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport='rest'
    )


@pytest.mark.parametrize("request_type", [
    lva_service.CreateProcessRequest,
    dict,
])
def test_create_process_rest(request_type):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport="rest",
    )

    # send a request that will satisfy transcoding
    request_init = {'parent': 'projects/sample1/locations/sample2/clusters/sample3'}
    request_init["process"] = {'name': 'name_value', 'create_time': {'seconds': 751, 'nanos': 543}, 'update_time': {}, 'analysis': 'analysis_value', 'attribute_overrides': ['attribute_overrides_value1', 'attribute_overrides_value2'], 'run_status': {'state': 1, 'reason': 'reason_value'}, 'run_mode': 1, 'event_id': 'event_id_value', 'batch_id': 'batch_id_value', 'retry_count': 1214}
    request = request_type(**request_init)

    # Mock the http request call within the method and fake a response.
    with mock.patch.object(type(client.transport._session), 'request') as req:
        # Designate an appropriate value for the returned response.
        return_value = operations_pb2.Operation(name='operations/spam')

        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 200
        json_return_value = json_format.MessageToJson(return_value)

        response_value._content = json_return_value.encode('UTF-8')
        req.return_value = response_value
        response = client.create_process(request)

    # Establish that the response is the type that we expect.
    assert response.operation.name == "operations/spam"


def test_create_process_rest_required_fields(request_type=lva_service.CreateProcessRequest):
    transport_class = transports.LiveVideoAnalyticsRestTransport

    request_init = {}
    request_init["parent"] = ""
    request_init["process_id"] = ""
    request = request_type(**request_init)
    pb_request = request_type.pb(request)
    jsonified_request = json.loads(json_format.MessageToJson(
        pb_request,
        including_default_value_fields=False,
        use_integers_for_enums=False
    ))

    # verify fields with default values are dropped
    assert "processId" not in jsonified_request

    unset_fields = transport_class(credentials=ga_credentials.AnonymousCredentials()).create_process._get_unset_required_fields(jsonified_request)
    jsonified_request.update(unset_fields)

    # verify required fields with default values are now present
    assert "processId" in jsonified_request
    assert jsonified_request["processId"] == request_init["process_id"]

    jsonified_request["parent"] = 'parent_value'
    jsonified_request["processId"] = 'process_id_value'

    unset_fields = transport_class(credentials=ga_credentials.AnonymousCredentials()).create_process._get_unset_required_fields(jsonified_request)
    # Check that path parameters and body parameters are not mixing in.
    assert not set(unset_fields) - set(("process_id", "request_id", ))
    jsonified_request.update(unset_fields)

    # verify required fields with non-default values are left alone
    assert "parent" in jsonified_request
    assert jsonified_request["parent"] == 'parent_value'
    assert "processId" in jsonified_request
    assert jsonified_request["processId"] == 'process_id_value'

    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport='rest',
    )
    request = request_type(**request_init)

    # Designate an appropriate value for the returned response.
    return_value = operations_pb2.Operation(name='operations/spam')
    # Mock the http request call within the method and fake a response.
    with mock.patch.object(Session, 'request') as req:
        # We need to mock transcode() because providing default values
        # for required fields will fail the real version if the http_options
        # expect actual values for those fields.
        with mock.patch.object(path_template, 'transcode') as transcode:
            # A uri without fields and an empty body will force all the
            # request fields to show up in the query_params.
            pb_request = request_type.pb(request)
            transcode_result = {
                'uri': 'v1/sample_method',
                'method': "post",
                'query_params': pb_request,
            }
            transcode_result['body'] = pb_request
            transcode.return_value = transcode_result

            response_value = Response()
            response_value.status_code = 200
            json_return_value = json_format.MessageToJson(return_value)

            response_value._content = json_return_value.encode('UTF-8')
            req.return_value = response_value

            response = client.create_process(request)

            expected_params = [
                (
                    "processId",
                    "",
                ),
            ]
            actual_params = req.call_args.kwargs['params']
            assert expected_params == actual_params


def test_create_process_rest_unset_required_fields():
    transport = transports.LiveVideoAnalyticsRestTransport(credentials=ga_credentials.AnonymousCredentials)

    unset_fields = transport.create_process._get_unset_required_fields({})
    assert set(unset_fields) == (set(("processId", "requestId", )) & set(("parent", "processId", "process", )))


@pytest.mark.parametrize("null_interceptor", [True, False])
def test_create_process_rest_interceptors(null_interceptor):
    transport = transports.LiveVideoAnalyticsRestTransport(
        credentials=ga_credentials.AnonymousCredentials(),
        interceptor=None if null_interceptor else transports.LiveVideoAnalyticsRestInterceptor(),
        )
    client = LiveVideoAnalyticsClient(transport=transport)
    with mock.patch.object(type(client.transport._session), "request") as req, \
         mock.patch.object(path_template, "transcode")  as transcode, \
         mock.patch.object(operation.Operation, "_set_result_from_operation"), \
         mock.patch.object(transports.LiveVideoAnalyticsRestInterceptor, "post_create_process") as post, \
         mock.patch.object(transports.LiveVideoAnalyticsRestInterceptor, "pre_create_process") as pre:
        pre.assert_not_called()
        post.assert_not_called()
        pb_message = lva_service.CreateProcessRequest.pb(lva_service.CreateProcessRequest())
        transcode.return_value = {
            "method": "post",
            "uri": "my_uri",
            "body": pb_message,
            "query_params": pb_message,
        }

        req.return_value = Response()
        req.return_value.status_code = 200
        req.return_value.request = PreparedRequest()
        req.return_value._content = json_format.MessageToJson(operations_pb2.Operation())

        request = lva_service.CreateProcessRequest()
        metadata =[
            ("key", "val"),
            ("cephalopod", "squid"),
        ]
        pre.return_value = request, metadata
        post.return_value = operations_pb2.Operation()

        client.create_process(request, metadata=[("key", "val"), ("cephalopod", "squid"),])

        pre.assert_called_once()
        post.assert_called_once()


def test_create_process_rest_bad_request(transport: str = 'rest', request_type=lva_service.CreateProcessRequest):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # send a request that will satisfy transcoding
    request_init = {'parent': 'projects/sample1/locations/sample2/clusters/sample3'}
    request_init["process"] = {'name': 'name_value', 'create_time': {'seconds': 751, 'nanos': 543}, 'update_time': {}, 'analysis': 'analysis_value', 'attribute_overrides': ['attribute_overrides_value1', 'attribute_overrides_value2'], 'run_status': {'state': 1, 'reason': 'reason_value'}, 'run_mode': 1, 'event_id': 'event_id_value', 'batch_id': 'batch_id_value', 'retry_count': 1214}
    request = request_type(**request_init)

    # Mock the http request call within the method and fake a BadRequest error.
    with mock.patch.object(Session, 'request') as req, pytest.raises(core_exceptions.BadRequest):
        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 400
        response_value.request = Request()
        req.return_value = response_value
        client.create_process(request)


def test_create_process_rest_flattened():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport="rest",
    )

    # Mock the http request call within the method and fake a response.
    with mock.patch.object(type(client.transport._session), 'request') as req:
        # Designate an appropriate value for the returned response.
        return_value = operations_pb2.Operation(name='operations/spam')

        # get arguments that satisfy an http rule for this method
        sample_request = {'parent': 'projects/sample1/locations/sample2/clusters/sample3'}

        # get truthy value for each flattened field
        mock_args = dict(
            parent='parent_value',
            process=lva_resources.Process(name='name_value'),
            process_id='process_id_value',
        )
        mock_args.update(sample_request)

        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 200
        json_return_value = json_format.MessageToJson(return_value)
        response_value._content = json_return_value.encode('UTF-8')
        req.return_value = response_value

        client.create_process(**mock_args)

        # Establish that the underlying call was made with the expected
        # request object values.
        assert len(req.mock_calls) == 1
        _, args, _ = req.mock_calls[0]
        assert path_template.validate("%s/v1/{parent=projects/*/locations/*/clusters/*}/processes" % client.transport._host, args[1])


def test_create_process_rest_flattened_error(transport: str = 'rest'):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # Attempting to call a method with both a request object and flattened
    # fields is an error.
    with pytest.raises(ValueError):
        client.create_process(
            lva_service.CreateProcessRequest(),
            parent='parent_value',
            process=lva_resources.Process(name='name_value'),
            process_id='process_id_value',
        )


def test_create_process_rest_error():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport='rest'
    )


@pytest.mark.parametrize("request_type", [
    lva_service.UpdateProcessRequest,
    dict,
])
def test_update_process_rest(request_type):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport="rest",
    )

    # send a request that will satisfy transcoding
    request_init = {'process': {'name': 'projects/sample1/locations/sample2/clusters/sample3/processes/sample4'}}
    request_init["process"] = {'name': 'projects/sample1/locations/sample2/clusters/sample3/processes/sample4', 'create_time': {'seconds': 751, 'nanos': 543}, 'update_time': {}, 'analysis': 'analysis_value', 'attribute_overrides': ['attribute_overrides_value1', 'attribute_overrides_value2'], 'run_status': {'state': 1, 'reason': 'reason_value'}, 'run_mode': 1, 'event_id': 'event_id_value', 'batch_id': 'batch_id_value', 'retry_count': 1214}
    request = request_type(**request_init)

    # Mock the http request call within the method and fake a response.
    with mock.patch.object(type(client.transport._session), 'request') as req:
        # Designate an appropriate value for the returned response.
        return_value = operations_pb2.Operation(name='operations/spam')

        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 200
        json_return_value = json_format.MessageToJson(return_value)

        response_value._content = json_return_value.encode('UTF-8')
        req.return_value = response_value
        response = client.update_process(request)

    # Establish that the response is the type that we expect.
    assert response.operation.name == "operations/spam"


def test_update_process_rest_required_fields(request_type=lva_service.UpdateProcessRequest):
    transport_class = transports.LiveVideoAnalyticsRestTransport

    request_init = {}
    request = request_type(**request_init)
    pb_request = request_type.pb(request)
    jsonified_request = json.loads(json_format.MessageToJson(
        pb_request,
        including_default_value_fields=False,
        use_integers_for_enums=False
    ))

    # verify fields with default values are dropped

    unset_fields = transport_class(credentials=ga_credentials.AnonymousCredentials()).update_process._get_unset_required_fields(jsonified_request)
    jsonified_request.update(unset_fields)

    # verify required fields with default values are now present

    unset_fields = transport_class(credentials=ga_credentials.AnonymousCredentials()).update_process._get_unset_required_fields(jsonified_request)
    # Check that path parameters and body parameters are not mixing in.
    assert not set(unset_fields) - set(("request_id", "update_mask", ))
    jsonified_request.update(unset_fields)

    # verify required fields with non-default values are left alone

    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport='rest',
    )
    request = request_type(**request_init)

    # Designate an appropriate value for the returned response.
    return_value = operations_pb2.Operation(name='operations/spam')
    # Mock the http request call within the method and fake a response.
    with mock.patch.object(Session, 'request') as req:
        # We need to mock transcode() because providing default values
        # for required fields will fail the real version if the http_options
        # expect actual values for those fields.
        with mock.patch.object(path_template, 'transcode') as transcode:
            # A uri without fields and an empty body will force all the
            # request fields to show up in the query_params.
            pb_request = request_type.pb(request)
            transcode_result = {
                'uri': 'v1/sample_method',
                'method': "patch",
                'query_params': pb_request,
            }
            transcode_result['body'] = pb_request
            transcode.return_value = transcode_result

            response_value = Response()
            response_value.status_code = 200
            json_return_value = json_format.MessageToJson(return_value)

            response_value._content = json_return_value.encode('UTF-8')
            req.return_value = response_value

            response = client.update_process(request)

            expected_params = [
            ]
            actual_params = req.call_args.kwargs['params']
            assert expected_params == actual_params


def test_update_process_rest_unset_required_fields():
    transport = transports.LiveVideoAnalyticsRestTransport(credentials=ga_credentials.AnonymousCredentials)

    unset_fields = transport.update_process._get_unset_required_fields({})
    assert set(unset_fields) == (set(("requestId", "updateMask", )) & set(("updateMask", "process", )))


@pytest.mark.parametrize("null_interceptor", [True, False])
def test_update_process_rest_interceptors(null_interceptor):
    transport = transports.LiveVideoAnalyticsRestTransport(
        credentials=ga_credentials.AnonymousCredentials(),
        interceptor=None if null_interceptor else transports.LiveVideoAnalyticsRestInterceptor(),
        )
    client = LiveVideoAnalyticsClient(transport=transport)
    with mock.patch.object(type(client.transport._session), "request") as req, \
         mock.patch.object(path_template, "transcode")  as transcode, \
         mock.patch.object(operation.Operation, "_set_result_from_operation"), \
         mock.patch.object(transports.LiveVideoAnalyticsRestInterceptor, "post_update_process") as post, \
         mock.patch.object(transports.LiveVideoAnalyticsRestInterceptor, "pre_update_process") as pre:
        pre.assert_not_called()
        post.assert_not_called()
        pb_message = lva_service.UpdateProcessRequest.pb(lva_service.UpdateProcessRequest())
        transcode.return_value = {
            "method": "post",
            "uri": "my_uri",
            "body": pb_message,
            "query_params": pb_message,
        }

        req.return_value = Response()
        req.return_value.status_code = 200
        req.return_value.request = PreparedRequest()
        req.return_value._content = json_format.MessageToJson(operations_pb2.Operation())

        request = lva_service.UpdateProcessRequest()
        metadata =[
            ("key", "val"),
            ("cephalopod", "squid"),
        ]
        pre.return_value = request, metadata
        post.return_value = operations_pb2.Operation()

        client.update_process(request, metadata=[("key", "val"), ("cephalopod", "squid"),])

        pre.assert_called_once()
        post.assert_called_once()


def test_update_process_rest_bad_request(transport: str = 'rest', request_type=lva_service.UpdateProcessRequest):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # send a request that will satisfy transcoding
    request_init = {'process': {'name': 'projects/sample1/locations/sample2/clusters/sample3/processes/sample4'}}
    request_init["process"] = {'name': 'projects/sample1/locations/sample2/clusters/sample3/processes/sample4', 'create_time': {'seconds': 751, 'nanos': 543}, 'update_time': {}, 'analysis': 'analysis_value', 'attribute_overrides': ['attribute_overrides_value1', 'attribute_overrides_value2'], 'run_status': {'state': 1, 'reason': 'reason_value'}, 'run_mode': 1, 'event_id': 'event_id_value', 'batch_id': 'batch_id_value', 'retry_count': 1214}
    request = request_type(**request_init)

    # Mock the http request call within the method and fake a BadRequest error.
    with mock.patch.object(Session, 'request') as req, pytest.raises(core_exceptions.BadRequest):
        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 400
        response_value.request = Request()
        req.return_value = response_value
        client.update_process(request)


def test_update_process_rest_flattened():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport="rest",
    )

    # Mock the http request call within the method and fake a response.
    with mock.patch.object(type(client.transport._session), 'request') as req:
        # Designate an appropriate value for the returned response.
        return_value = operations_pb2.Operation(name='operations/spam')

        # get arguments that satisfy an http rule for this method
        sample_request = {'process': {'name': 'projects/sample1/locations/sample2/clusters/sample3/processes/sample4'}}

        # get truthy value for each flattened field
        mock_args = dict(
            process=lva_resources.Process(name='name_value'),
            update_mask=field_mask_pb2.FieldMask(paths=['paths_value']),
        )
        mock_args.update(sample_request)

        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 200
        json_return_value = json_format.MessageToJson(return_value)
        response_value._content = json_return_value.encode('UTF-8')
        req.return_value = response_value

        client.update_process(**mock_args)

        # Establish that the underlying call was made with the expected
        # request object values.
        assert len(req.mock_calls) == 1
        _, args, _ = req.mock_calls[0]
        assert path_template.validate("%s/v1/{process.name=projects/*/locations/*/clusters/*/processes/*}" % client.transport._host, args[1])


def test_update_process_rest_flattened_error(transport: str = 'rest'):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # Attempting to call a method with both a request object and flattened
    # fields is an error.
    with pytest.raises(ValueError):
        client.update_process(
            lva_service.UpdateProcessRequest(),
            process=lva_resources.Process(name='name_value'),
            update_mask=field_mask_pb2.FieldMask(paths=['paths_value']),
        )


def test_update_process_rest_error():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport='rest'
    )


@pytest.mark.parametrize("request_type", [
    lva_service.DeleteProcessRequest,
    dict,
])
def test_delete_process_rest(request_type):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport="rest",
    )

    # send a request that will satisfy transcoding
    request_init = {'name': 'projects/sample1/locations/sample2/clusters/sample3/processes/sample4'}
    request = request_type(**request_init)

    # Mock the http request call within the method and fake a response.
    with mock.patch.object(type(client.transport._session), 'request') as req:
        # Designate an appropriate value for the returned response.
        return_value = operations_pb2.Operation(name='operations/spam')

        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 200
        json_return_value = json_format.MessageToJson(return_value)

        response_value._content = json_return_value.encode('UTF-8')
        req.return_value = response_value
        response = client.delete_process(request)

    # Establish that the response is the type that we expect.
    assert response.operation.name == "operations/spam"


def test_delete_process_rest_required_fields(request_type=lva_service.DeleteProcessRequest):
    transport_class = transports.LiveVideoAnalyticsRestTransport

    request_init = {}
    request_init["name"] = ""
    request = request_type(**request_init)
    pb_request = request_type.pb(request)
    jsonified_request = json.loads(json_format.MessageToJson(
        pb_request,
        including_default_value_fields=False,
        use_integers_for_enums=False
    ))

    # verify fields with default values are dropped

    unset_fields = transport_class(credentials=ga_credentials.AnonymousCredentials()).delete_process._get_unset_required_fields(jsonified_request)
    jsonified_request.update(unset_fields)

    # verify required fields with default values are now present

    jsonified_request["name"] = 'name_value'

    unset_fields = transport_class(credentials=ga_credentials.AnonymousCredentials()).delete_process._get_unset_required_fields(jsonified_request)
    # Check that path parameters and body parameters are not mixing in.
    assert not set(unset_fields) - set(("request_id", ))
    jsonified_request.update(unset_fields)

    # verify required fields with non-default values are left alone
    assert "name" in jsonified_request
    assert jsonified_request["name"] == 'name_value'

    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport='rest',
    )
    request = request_type(**request_init)

    # Designate an appropriate value for the returned response.
    return_value = operations_pb2.Operation(name='operations/spam')
    # Mock the http request call within the method and fake a response.
    with mock.patch.object(Session, 'request') as req:
        # We need to mock transcode() because providing default values
        # for required fields will fail the real version if the http_options
        # expect actual values for those fields.
        with mock.patch.object(path_template, 'transcode') as transcode:
            # A uri without fields and an empty body will force all the
            # request fields to show up in the query_params.
            pb_request = request_type.pb(request)
            transcode_result = {
                'uri': 'v1/sample_method',
                'method': "delete",
                'query_params': pb_request,
            }
            transcode.return_value = transcode_result

            response_value = Response()
            response_value.status_code = 200
            json_return_value = json_format.MessageToJson(return_value)

            response_value._content = json_return_value.encode('UTF-8')
            req.return_value = response_value

            response = client.delete_process(request)

            expected_params = [
            ]
            actual_params = req.call_args.kwargs['params']
            assert expected_params == actual_params


def test_delete_process_rest_unset_required_fields():
    transport = transports.LiveVideoAnalyticsRestTransport(credentials=ga_credentials.AnonymousCredentials)

    unset_fields = transport.delete_process._get_unset_required_fields({})
    assert set(unset_fields) == (set(("requestId", )) & set(("name", )))


@pytest.mark.parametrize("null_interceptor", [True, False])
def test_delete_process_rest_interceptors(null_interceptor):
    transport = transports.LiveVideoAnalyticsRestTransport(
        credentials=ga_credentials.AnonymousCredentials(),
        interceptor=None if null_interceptor else transports.LiveVideoAnalyticsRestInterceptor(),
        )
    client = LiveVideoAnalyticsClient(transport=transport)
    with mock.patch.object(type(client.transport._session), "request") as req, \
         mock.patch.object(path_template, "transcode")  as transcode, \
         mock.patch.object(operation.Operation, "_set_result_from_operation"), \
         mock.patch.object(transports.LiveVideoAnalyticsRestInterceptor, "post_delete_process") as post, \
         mock.patch.object(transports.LiveVideoAnalyticsRestInterceptor, "pre_delete_process") as pre:
        pre.assert_not_called()
        post.assert_not_called()
        pb_message = lva_service.DeleteProcessRequest.pb(lva_service.DeleteProcessRequest())
        transcode.return_value = {
            "method": "post",
            "uri": "my_uri",
            "body": pb_message,
            "query_params": pb_message,
        }

        req.return_value = Response()
        req.return_value.status_code = 200
        req.return_value.request = PreparedRequest()
        req.return_value._content = json_format.MessageToJson(operations_pb2.Operation())

        request = lva_service.DeleteProcessRequest()
        metadata =[
            ("key", "val"),
            ("cephalopod", "squid"),
        ]
        pre.return_value = request, metadata
        post.return_value = operations_pb2.Operation()

        client.delete_process(request, metadata=[("key", "val"), ("cephalopod", "squid"),])

        pre.assert_called_once()
        post.assert_called_once()


def test_delete_process_rest_bad_request(transport: str = 'rest', request_type=lva_service.DeleteProcessRequest):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # send a request that will satisfy transcoding
    request_init = {'name': 'projects/sample1/locations/sample2/clusters/sample3/processes/sample4'}
    request = request_type(**request_init)

    # Mock the http request call within the method and fake a BadRequest error.
    with mock.patch.object(Session, 'request') as req, pytest.raises(core_exceptions.BadRequest):
        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 400
        response_value.request = Request()
        req.return_value = response_value
        client.delete_process(request)


def test_delete_process_rest_flattened():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport="rest",
    )

    # Mock the http request call within the method and fake a response.
    with mock.patch.object(type(client.transport._session), 'request') as req:
        # Designate an appropriate value for the returned response.
        return_value = operations_pb2.Operation(name='operations/spam')

        # get arguments that satisfy an http rule for this method
        sample_request = {'name': 'projects/sample1/locations/sample2/clusters/sample3/processes/sample4'}

        # get truthy value for each flattened field
        mock_args = dict(
            name='name_value',
        )
        mock_args.update(sample_request)

        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 200
        json_return_value = json_format.MessageToJson(return_value)
        response_value._content = json_return_value.encode('UTF-8')
        req.return_value = response_value

        client.delete_process(**mock_args)

        # Establish that the underlying call was made with the expected
        # request object values.
        assert len(req.mock_calls) == 1
        _, args, _ = req.mock_calls[0]
        assert path_template.validate("%s/v1/{name=projects/*/locations/*/clusters/*/processes/*}" % client.transport._host, args[1])


def test_delete_process_rest_flattened_error(transport: str = 'rest'):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # Attempting to call a method with both a request object and flattened
    # fields is an error.
    with pytest.raises(ValueError):
        client.delete_process(
            lva_service.DeleteProcessRequest(),
            name='name_value',
        )


def test_delete_process_rest_error():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport='rest'
    )


@pytest.mark.parametrize("request_type", [
    lva_service.BatchRunProcessRequest,
    dict,
])
def test_batch_run_process_rest(request_type):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport="rest",
    )

    # send a request that will satisfy transcoding
    request_init = {'parent': 'projects/sample1/locations/sample2/clusters/sample3'}
    request = request_type(**request_init)

    # Mock the http request call within the method and fake a response.
    with mock.patch.object(type(client.transport._session), 'request') as req:
        # Designate an appropriate value for the returned response.
        return_value = operations_pb2.Operation(name='operations/spam')

        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 200
        json_return_value = json_format.MessageToJson(return_value)

        response_value._content = json_return_value.encode('UTF-8')
        req.return_value = response_value
        response = client.batch_run_process(request)

    # Establish that the response is the type that we expect.
    assert response.operation.name == "operations/spam"


def test_batch_run_process_rest_required_fields(request_type=lva_service.BatchRunProcessRequest):
    transport_class = transports.LiveVideoAnalyticsRestTransport

    request_init = {}
    request_init["parent"] = ""
    request = request_type(**request_init)
    pb_request = request_type.pb(request)
    jsonified_request = json.loads(json_format.MessageToJson(
        pb_request,
        including_default_value_fields=False,
        use_integers_for_enums=False
    ))

    # verify fields with default values are dropped

    unset_fields = transport_class(credentials=ga_credentials.AnonymousCredentials()).batch_run_process._get_unset_required_fields(jsonified_request)
    jsonified_request.update(unset_fields)

    # verify required fields with default values are now present

    jsonified_request["parent"] = 'parent_value'

    unset_fields = transport_class(credentials=ga_credentials.AnonymousCredentials()).batch_run_process._get_unset_required_fields(jsonified_request)
    jsonified_request.update(unset_fields)

    # verify required fields with non-default values are left alone
    assert "parent" in jsonified_request
    assert jsonified_request["parent"] == 'parent_value'

    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport='rest',
    )
    request = request_type(**request_init)

    # Designate an appropriate value for the returned response.
    return_value = operations_pb2.Operation(name='operations/spam')
    # Mock the http request call within the method and fake a response.
    with mock.patch.object(Session, 'request') as req:
        # We need to mock transcode() because providing default values
        # for required fields will fail the real version if the http_options
        # expect actual values for those fields.
        with mock.patch.object(path_template, 'transcode') as transcode:
            # A uri without fields and an empty body will force all the
            # request fields to show up in the query_params.
            pb_request = request_type.pb(request)
            transcode_result = {
                'uri': 'v1/sample_method',
                'method': "post",
                'query_params': pb_request,
            }
            transcode_result['body'] = pb_request
            transcode.return_value = transcode_result

            response_value = Response()
            response_value.status_code = 200
            json_return_value = json_format.MessageToJson(return_value)

            response_value._content = json_return_value.encode('UTF-8')
            req.return_value = response_value

            response = client.batch_run_process(request)

            expected_params = [
            ]
            actual_params = req.call_args.kwargs['params']
            assert expected_params == actual_params


def test_batch_run_process_rest_unset_required_fields():
    transport = transports.LiveVideoAnalyticsRestTransport(credentials=ga_credentials.AnonymousCredentials)

    unset_fields = transport.batch_run_process._get_unset_required_fields({})
    assert set(unset_fields) == (set(()) & set(("parent", "requests", )))


@pytest.mark.parametrize("null_interceptor", [True, False])
def test_batch_run_process_rest_interceptors(null_interceptor):
    transport = transports.LiveVideoAnalyticsRestTransport(
        credentials=ga_credentials.AnonymousCredentials(),
        interceptor=None if null_interceptor else transports.LiveVideoAnalyticsRestInterceptor(),
        )
    client = LiveVideoAnalyticsClient(transport=transport)
    with mock.patch.object(type(client.transport._session), "request") as req, \
         mock.patch.object(path_template, "transcode")  as transcode, \
         mock.patch.object(operation.Operation, "_set_result_from_operation"), \
         mock.patch.object(transports.LiveVideoAnalyticsRestInterceptor, "post_batch_run_process") as post, \
         mock.patch.object(transports.LiveVideoAnalyticsRestInterceptor, "pre_batch_run_process") as pre:
        pre.assert_not_called()
        post.assert_not_called()
        pb_message = lva_service.BatchRunProcessRequest.pb(lva_service.BatchRunProcessRequest())
        transcode.return_value = {
            "method": "post",
            "uri": "my_uri",
            "body": pb_message,
            "query_params": pb_message,
        }

        req.return_value = Response()
        req.return_value.status_code = 200
        req.return_value.request = PreparedRequest()
        req.return_value._content = json_format.MessageToJson(operations_pb2.Operation())

        request = lva_service.BatchRunProcessRequest()
        metadata =[
            ("key", "val"),
            ("cephalopod", "squid"),
        ]
        pre.return_value = request, metadata
        post.return_value = operations_pb2.Operation()

        client.batch_run_process(request, metadata=[("key", "val"), ("cephalopod", "squid"),])

        pre.assert_called_once()
        post.assert_called_once()


def test_batch_run_process_rest_bad_request(transport: str = 'rest', request_type=lva_service.BatchRunProcessRequest):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # send a request that will satisfy transcoding
    request_init = {'parent': 'projects/sample1/locations/sample2/clusters/sample3'}
    request = request_type(**request_init)

    # Mock the http request call within the method and fake a BadRequest error.
    with mock.patch.object(Session, 'request') as req, pytest.raises(core_exceptions.BadRequest):
        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 400
        response_value.request = Request()
        req.return_value = response_value
        client.batch_run_process(request)


def test_batch_run_process_rest_flattened():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport="rest",
    )

    # Mock the http request call within the method and fake a response.
    with mock.patch.object(type(client.transport._session), 'request') as req:
        # Designate an appropriate value for the returned response.
        return_value = operations_pb2.Operation(name='operations/spam')

        # get arguments that satisfy an http rule for this method
        sample_request = {'parent': 'projects/sample1/locations/sample2/clusters/sample3'}

        # get truthy value for each flattened field
        mock_args = dict(
            parent='parent_value',
            requests=[lva_service.CreateProcessRequest(parent='parent_value')],
        )
        mock_args.update(sample_request)

        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 200
        json_return_value = json_format.MessageToJson(return_value)
        response_value._content = json_return_value.encode('UTF-8')
        req.return_value = response_value

        client.batch_run_process(**mock_args)

        # Establish that the underlying call was made with the expected
        # request object values.
        assert len(req.mock_calls) == 1
        _, args, _ = req.mock_calls[0]
        assert path_template.validate("%s/v1/{parent=projects/*/locations/*/clusters/*}/processes:batchRun" % client.transport._host, args[1])


def test_batch_run_process_rest_flattened_error(transport: str = 'rest'):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    # Attempting to call a method with both a request object and flattened
    # fields is an error.
    with pytest.raises(ValueError):
        client.batch_run_process(
            lva_service.BatchRunProcessRequest(),
            parent='parent_value',
            requests=[lva_service.CreateProcessRequest(parent='parent_value')],
        )


def test_batch_run_process_rest_error():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport='rest'
    )


def test_credentials_transport_error():
    # It is an error to provide credentials and a transport instance.
    transport = transports.LiveVideoAnalyticsGrpcTransport(
        credentials=ga_credentials.AnonymousCredentials(),
    )
    with pytest.raises(ValueError):
        client = LiveVideoAnalyticsClient(
            credentials=ga_credentials.AnonymousCredentials(),
            transport=transport,
        )

    # It is an error to provide a credentials file and a transport instance.
    transport = transports.LiveVideoAnalyticsGrpcTransport(
        credentials=ga_credentials.AnonymousCredentials(),
    )
    with pytest.raises(ValueError):
        client = LiveVideoAnalyticsClient(
            client_options={"credentials_file": "credentials.json"},
            transport=transport,
        )

    # It is an error to provide an api_key and a transport instance.
    transport = transports.LiveVideoAnalyticsGrpcTransport(
        credentials=ga_credentials.AnonymousCredentials(),
    )
    options = client_options.ClientOptions()
    options.api_key = "api_key"
    with pytest.raises(ValueError):
        client = LiveVideoAnalyticsClient(
            client_options=options,
            transport=transport,
        )

    # It is an error to provide an api_key and a credential.
    options = mock.Mock()
    options.api_key = "api_key"
    with pytest.raises(ValueError):
        client = LiveVideoAnalyticsClient(
            client_options=options,
            credentials=ga_credentials.AnonymousCredentials()
        )

    # It is an error to provide scopes and a transport instance.
    transport = transports.LiveVideoAnalyticsGrpcTransport(
        credentials=ga_credentials.AnonymousCredentials(),
    )
    with pytest.raises(ValueError):
        client = LiveVideoAnalyticsClient(
            client_options={"scopes": ["1", "2"]},
            transport=transport,
        )


def test_transport_instance():
    # A client may be instantiated with a custom transport instance.
    transport = transports.LiveVideoAnalyticsGrpcTransport(
        credentials=ga_credentials.AnonymousCredentials(),
    )
    client = LiveVideoAnalyticsClient(transport=transport)
    assert client.transport is transport

def test_transport_get_channel():
    # A client may be instantiated with a custom transport instance.
    transport = transports.LiveVideoAnalyticsGrpcTransport(
        credentials=ga_credentials.AnonymousCredentials(),
    )
    channel = transport.grpc_channel
    assert channel

    transport = transports.LiveVideoAnalyticsGrpcAsyncIOTransport(
        credentials=ga_credentials.AnonymousCredentials(),
    )
    channel = transport.grpc_channel
    assert channel

@pytest.mark.parametrize("transport_class", [
    transports.LiveVideoAnalyticsGrpcTransport,
    transports.LiveVideoAnalyticsGrpcAsyncIOTransport,
    transports.LiveVideoAnalyticsRestTransport,
])
def test_transport_adc(transport_class):
    # Test default credentials are used if not provided.
    with mock.patch.object(google.auth, 'default') as adc:
        adc.return_value = (ga_credentials.AnonymousCredentials(), None)
        transport_class()
        adc.assert_called_once()

@pytest.mark.parametrize("transport_name", [
    "grpc",
    "rest",
])
def test_transport_kind(transport_name):
    transport = LiveVideoAnalyticsClient.get_transport_class(transport_name)(
        credentials=ga_credentials.AnonymousCredentials(),
    )
    assert transport.kind == transport_name

def test_transport_grpc_default():
    # A client should use the gRPC transport by default.
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )
    assert isinstance(
        client.transport,
        transports.LiveVideoAnalyticsGrpcTransport,
    )

def test_live_video_analytics_base_transport_error():
    # Passing both a credentials object and credentials_file should raise an error
    with pytest.raises(core_exceptions.DuplicateCredentialArgs):
        transport = transports.LiveVideoAnalyticsTransport(
            credentials=ga_credentials.AnonymousCredentials(),
            credentials_file="credentials.json"
        )


def test_live_video_analytics_base_transport():
    # Instantiate the base transport.
    with mock.patch('visionai.python.gapic.visionai.visionai_v1.services.live_video_analytics.transports.LiveVideoAnalyticsTransport.__init__') as Transport:
        Transport.return_value = None
        transport = transports.LiveVideoAnalyticsTransport(
            credentials=ga_credentials.AnonymousCredentials(),
        )

    # Every method on the transport should just blindly
    # raise NotImplementedError.
    methods = (
        'resolve_operator_info',
        'list_operators',
        'get_operator',
        'create_operator',
        'update_operator',
        'delete_operator',
        'list_analyses',
        'get_analysis',
        'create_analysis',
        'update_analysis',
        'delete_analysis',
        'list_processes',
        'get_process',
        'create_process',
        'update_process',
        'delete_process',
        'batch_run_process',
        'get_operation',
        'cancel_operation',
        'delete_operation',
        'list_operations',
    )
    for method in methods:
        with pytest.raises(NotImplementedError):
            getattr(transport, method)(request=object())

    with pytest.raises(NotImplementedError):
        transport.close()

    # Additionally, the LRO client (a property) should
    # also raise NotImplementedError
    with pytest.raises(NotImplementedError):
        transport.operations_client

    # Catch all for all remaining methods and properties
    remainder = [
        'kind',
    ]
    for r in remainder:
        with pytest.raises(NotImplementedError):
            getattr(transport, r)()


def test_live_video_analytics_base_transport_with_credentials_file():
    # Instantiate the base transport with a credentials file
    with mock.patch.object(google.auth, 'load_credentials_from_file', autospec=True) as load_creds, mock.patch('visionai.python.gapic.visionai.visionai_v1.services.live_video_analytics.transports.LiveVideoAnalyticsTransport._prep_wrapped_messages') as Transport:
        Transport.return_value = None
        load_creds.return_value = (ga_credentials.AnonymousCredentials(), None)
        transport = transports.LiveVideoAnalyticsTransport(
            credentials_file="credentials.json",
            quota_project_id="octopus",
        )
        load_creds.assert_called_once_with("credentials.json",
            scopes=None,
            default_scopes=(
            'https://www.googleapis.com/auth/cloud-platform',
),
            quota_project_id="octopus",
        )


def test_live_video_analytics_base_transport_with_adc():
    # Test the default credentials are used if credentials and credentials_file are None.
    with mock.patch.object(google.auth, 'default', autospec=True) as adc, mock.patch('visionai.python.gapic.visionai.visionai_v1.services.live_video_analytics.transports.LiveVideoAnalyticsTransport._prep_wrapped_messages') as Transport:
        Transport.return_value = None
        adc.return_value = (ga_credentials.AnonymousCredentials(), None)
        transport = transports.LiveVideoAnalyticsTransport()
        adc.assert_called_once()


def test_live_video_analytics_auth_adc():
    # If no credentials are provided, we should use ADC credentials.
    with mock.patch.object(google.auth, 'default', autospec=True) as adc:
        adc.return_value = (ga_credentials.AnonymousCredentials(), None)
        LiveVideoAnalyticsClient()
        adc.assert_called_once_with(
            scopes=None,
            default_scopes=(
            'https://www.googleapis.com/auth/cloud-platform',
),
            quota_project_id=None,
        )


@pytest.mark.parametrize(
    "transport_class",
    [
        transports.LiveVideoAnalyticsGrpcTransport,
        transports.LiveVideoAnalyticsGrpcAsyncIOTransport,
    ],
)
def test_live_video_analytics_transport_auth_adc(transport_class):
    # If credentials and host are not provided, the transport class should use
    # ADC credentials.
    with mock.patch.object(google.auth, 'default', autospec=True) as adc:
        adc.return_value = (ga_credentials.AnonymousCredentials(), None)
        transport_class(quota_project_id="octopus", scopes=["1", "2"])
        adc.assert_called_once_with(
            scopes=["1", "2"],
            default_scopes=(                'https://www.googleapis.com/auth/cloud-platform',),
            quota_project_id="octopus",
        )


@pytest.mark.parametrize(
    "transport_class",
    [
        transports.LiveVideoAnalyticsGrpcTransport,
        transports.LiveVideoAnalyticsGrpcAsyncIOTransport,
        transports.LiveVideoAnalyticsRestTransport,
    ],
)
def test_live_video_analytics_transport_auth_gdch_credentials(transport_class):
    host = 'https://language.com'
    api_audience_tests = [None, 'https://language2.com']
    api_audience_expect = [host, 'https://language2.com']
    for t, e in zip(api_audience_tests, api_audience_expect):
        with mock.patch.object(google.auth, 'default', autospec=True) as adc:
            gdch_mock = mock.MagicMock()
            type(gdch_mock).with_gdch_audience = mock.PropertyMock(return_value=gdch_mock)
            adc.return_value = (gdch_mock, None)
            transport_class(host=host, api_audience=t)
            gdch_mock.with_gdch_audience.assert_called_once_with(
                e
            )


@pytest.mark.parametrize(
    "transport_class,grpc_helpers",
    [
        (transports.LiveVideoAnalyticsGrpcTransport, grpc_helpers),
        (transports.LiveVideoAnalyticsGrpcAsyncIOTransport, grpc_helpers_async)
    ],
)
def test_live_video_analytics_transport_create_channel(transport_class, grpc_helpers):
    # If credentials and host are not provided, the transport class should use
    # ADC credentials.
    with mock.patch.object(google.auth, "default", autospec=True) as adc, mock.patch.object(
        grpc_helpers, "create_channel", autospec=True
    ) as create_channel:
        creds = ga_credentials.AnonymousCredentials()
        adc.return_value = (creds, None)
        transport_class(
            quota_project_id="octopus",
            scopes=["1", "2"]
        )

        create_channel.assert_called_with(
            "visionai.googleapis.com:443",
            credentials=creds,
            credentials_file=None,
            quota_project_id="octopus",
            default_scopes=(
                'https://www.googleapis.com/auth/cloud-platform',
),
            scopes=["1", "2"],
            default_host="visionai.googleapis.com",
            ssl_credentials=None,
            options=[
                ("grpc.max_send_message_length", -1),
                ("grpc.max_receive_message_length", -1),
            ],
        )


@pytest.mark.parametrize("transport_class", [transports.LiveVideoAnalyticsGrpcTransport, transports.LiveVideoAnalyticsGrpcAsyncIOTransport])
def test_live_video_analytics_grpc_transport_client_cert_source_for_mtls(
    transport_class
):
    cred = ga_credentials.AnonymousCredentials()

    # Check ssl_channel_credentials is used if provided.
    with mock.patch.object(transport_class, "create_channel") as mock_create_channel:
        mock_ssl_channel_creds = mock.Mock()
        transport_class(
            host="squid.clam.whelk",
            credentials=cred,
            ssl_channel_credentials=mock_ssl_channel_creds
        )
        mock_create_channel.assert_called_once_with(
            "squid.clam.whelk:443",
            credentials=cred,
            credentials_file=None,
            scopes=None,
            ssl_credentials=mock_ssl_channel_creds,
            quota_project_id=None,
            options=[
                ("grpc.max_send_message_length", -1),
                ("grpc.max_receive_message_length", -1),
            ],
        )

    # Check if ssl_channel_credentials is not provided, then client_cert_source_for_mtls
    # is used.
    with mock.patch.object(transport_class, "create_channel", return_value=mock.Mock()):
        with mock.patch("grpc.ssl_channel_credentials") as mock_ssl_cred:
            transport_class(
                credentials=cred,
                client_cert_source_for_mtls=client_cert_source_callback
            )
            expected_cert, expected_key = client_cert_source_callback()
            mock_ssl_cred.assert_called_once_with(
                certificate_chain=expected_cert,
                private_key=expected_key
            )

def test_live_video_analytics_http_transport_client_cert_source_for_mtls():
    cred = ga_credentials.AnonymousCredentials()
    with mock.patch("google.auth.transport.requests.AuthorizedSession.configure_mtls_channel") as mock_configure_mtls_channel:
        transports.LiveVideoAnalyticsRestTransport (
            credentials=cred,
            client_cert_source_for_mtls=client_cert_source_callback
        )
        mock_configure_mtls_channel.assert_called_once_with(client_cert_source_callback)


def test_live_video_analytics_rest_lro_client():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport='rest',
    )
    transport = client.transport

    # Ensure that we have a api-core operations client.
    assert isinstance(
        transport.operations_client,
        operations_v1.AbstractOperationsClient,
    )

    # Ensure that subsequent calls to the property send the exact same object.
    assert transport.operations_client is transport.operations_client


@pytest.mark.parametrize("transport_name", [
    "grpc",
    "grpc_asyncio",
    "rest",
])
def test_live_video_analytics_host_no_port(transport_name):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        client_options=client_options.ClientOptions(api_endpoint='visionai.googleapis.com'),
         transport=transport_name,
    )
    assert client.transport._host == (
        'visionai.googleapis.com:443'
        if transport_name in ['grpc', 'grpc_asyncio']
        else 'https://visionai.googleapis.com'
    )

@pytest.mark.parametrize("transport_name", [
    "grpc",
    "grpc_asyncio",
    "rest",
])
def test_live_video_analytics_host_with_port(transport_name):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        client_options=client_options.ClientOptions(api_endpoint='visionai.googleapis.com:8000'),
        transport=transport_name,
    )
    assert client.transport._host == (
        'visionai.googleapis.com:8000'
        if transport_name in ['grpc', 'grpc_asyncio']
        else 'https://visionai.googleapis.com:8000'
    )

@pytest.mark.parametrize("transport_name", [
    "rest",
])
def test_live_video_analytics_client_transport_session_collision(transport_name):
    creds1 = ga_credentials.AnonymousCredentials()
    creds2 = ga_credentials.AnonymousCredentials()
    client1 = LiveVideoAnalyticsClient(
        credentials=creds1,
        transport=transport_name,
    )
    client2 = LiveVideoAnalyticsClient(
        credentials=creds2,
        transport=transport_name,
    )
    session1 = client1.transport.resolve_operator_info._session
    session2 = client2.transport.resolve_operator_info._session
    assert session1 != session2
    session1 = client1.transport.list_operators._session
    session2 = client2.transport.list_operators._session
    assert session1 != session2
    session1 = client1.transport.get_operator._session
    session2 = client2.transport.get_operator._session
    assert session1 != session2
    session1 = client1.transport.create_operator._session
    session2 = client2.transport.create_operator._session
    assert session1 != session2
    session1 = client1.transport.update_operator._session
    session2 = client2.transport.update_operator._session
    assert session1 != session2
    session1 = client1.transport.delete_operator._session
    session2 = client2.transport.delete_operator._session
    assert session1 != session2
    session1 = client1.transport.list_analyses._session
    session2 = client2.transport.list_analyses._session
    assert session1 != session2
    session1 = client1.transport.get_analysis._session
    session2 = client2.transport.get_analysis._session
    assert session1 != session2
    session1 = client1.transport.create_analysis._session
    session2 = client2.transport.create_analysis._session
    assert session1 != session2
    session1 = client1.transport.update_analysis._session
    session2 = client2.transport.update_analysis._session
    assert session1 != session2
    session1 = client1.transport.delete_analysis._session
    session2 = client2.transport.delete_analysis._session
    assert session1 != session2
    session1 = client1.transport.list_processes._session
    session2 = client2.transport.list_processes._session
    assert session1 != session2
    session1 = client1.transport.get_process._session
    session2 = client2.transport.get_process._session
    assert session1 != session2
    session1 = client1.transport.create_process._session
    session2 = client2.transport.create_process._session
    assert session1 != session2
    session1 = client1.transport.update_process._session
    session2 = client2.transport.update_process._session
    assert session1 != session2
    session1 = client1.transport.delete_process._session
    session2 = client2.transport.delete_process._session
    assert session1 != session2
    session1 = client1.transport.batch_run_process._session
    session2 = client2.transport.batch_run_process._session
    assert session1 != session2
def test_live_video_analytics_grpc_transport_channel():
    channel = grpc.secure_channel('http://localhost/', grpc.local_channel_credentials())

    # Check that channel is used if provided.
    transport = transports.LiveVideoAnalyticsGrpcTransport(
        host="squid.clam.whelk",
        channel=channel,
    )
    assert transport.grpc_channel == channel
    assert transport._host == "squid.clam.whelk:443"
    assert transport._ssl_channel_credentials == None


def test_live_video_analytics_grpc_asyncio_transport_channel():
    channel = aio.secure_channel('http://localhost/', grpc.local_channel_credentials())

    # Check that channel is used if provided.
    transport = transports.LiveVideoAnalyticsGrpcAsyncIOTransport(
        host="squid.clam.whelk",
        channel=channel,
    )
    assert transport.grpc_channel == channel
    assert transport._host == "squid.clam.whelk:443"
    assert transport._ssl_channel_credentials == None


# Remove this test when deprecated arguments (api_mtls_endpoint, client_cert_source) are
# removed from grpc/grpc_asyncio transport constructor.
@pytest.mark.parametrize("transport_class", [transports.LiveVideoAnalyticsGrpcTransport, transports.LiveVideoAnalyticsGrpcAsyncIOTransport])
def test_live_video_analytics_transport_channel_mtls_with_client_cert_source(
    transport_class
):
    with mock.patch("grpc.ssl_channel_credentials", autospec=True) as grpc_ssl_channel_cred:
        with mock.patch.object(transport_class, "create_channel") as grpc_create_channel:
            mock_ssl_cred = mock.Mock()
            grpc_ssl_channel_cred.return_value = mock_ssl_cred

            mock_grpc_channel = mock.Mock()
            grpc_create_channel.return_value = mock_grpc_channel

            cred = ga_credentials.AnonymousCredentials()
            with pytest.warns(DeprecationWarning):
                with mock.patch.object(google.auth, 'default') as adc:
                    adc.return_value = (cred, None)
                    transport = transport_class(
                        host="squid.clam.whelk",
                        api_mtls_endpoint="mtls.squid.clam.whelk",
                        client_cert_source=client_cert_source_callback,
                    )
                    adc.assert_called_once()

            grpc_ssl_channel_cred.assert_called_once_with(
                certificate_chain=b"cert bytes", private_key=b"key bytes"
            )
            grpc_create_channel.assert_called_once_with(
                "mtls.squid.clam.whelk:443",
                credentials=cred,
                credentials_file=None,
                scopes=None,
                ssl_credentials=mock_ssl_cred,
                quota_project_id=None,
                options=[
                    ("grpc.max_send_message_length", -1),
                    ("grpc.max_receive_message_length", -1),
                ],
            )
            assert transport.grpc_channel == mock_grpc_channel
            assert transport._ssl_channel_credentials == mock_ssl_cred


# Remove this test when deprecated arguments (api_mtls_endpoint, client_cert_source) are
# removed from grpc/grpc_asyncio transport constructor.
@pytest.mark.parametrize("transport_class", [transports.LiveVideoAnalyticsGrpcTransport, transports.LiveVideoAnalyticsGrpcAsyncIOTransport])
def test_live_video_analytics_transport_channel_mtls_with_adc(
    transport_class
):
    mock_ssl_cred = mock.Mock()
    with mock.patch.multiple(
        "google.auth.transport.grpc.SslCredentials",
        __init__=mock.Mock(return_value=None),
        ssl_credentials=mock.PropertyMock(return_value=mock_ssl_cred),
    ):
        with mock.patch.object(transport_class, "create_channel") as grpc_create_channel:
            mock_grpc_channel = mock.Mock()
            grpc_create_channel.return_value = mock_grpc_channel
            mock_cred = mock.Mock()

            with pytest.warns(DeprecationWarning):
                transport = transport_class(
                    host="squid.clam.whelk",
                    credentials=mock_cred,
                    api_mtls_endpoint="mtls.squid.clam.whelk",
                    client_cert_source=None,
                )

            grpc_create_channel.assert_called_once_with(
                "mtls.squid.clam.whelk:443",
                credentials=mock_cred,
                credentials_file=None,
                scopes=None,
                ssl_credentials=mock_ssl_cred,
                quota_project_id=None,
                options=[
                    ("grpc.max_send_message_length", -1),
                    ("grpc.max_receive_message_length", -1),
                ],
            )
            assert transport.grpc_channel == mock_grpc_channel


def test_live_video_analytics_grpc_lro_client():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport='grpc',
    )
    transport = client.transport

    # Ensure that we have a api-core operations client.
    assert isinstance(
        transport.operations_client,
        operations_v1.OperationsClient,
    )

    # Ensure that subsequent calls to the property send the exact same object.
    assert transport.operations_client is transport.operations_client


def test_live_video_analytics_grpc_lro_async_client():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport='grpc_asyncio',
    )
    transport = client.transport

    # Ensure that we have a api-core operations client.
    assert isinstance(
        transport.operations_client,
        operations_v1.OperationsAsyncClient,
    )

    # Ensure that subsequent calls to the property send the exact same object.
    assert transport.operations_client is transport.operations_client


def test_analysis_path():
    project = "squid"
    location = "clam"
    cluster = "whelk"
    analysis = "octopus"
    expected = "projects/{project}/locations/{location}/clusters/{cluster}/analyses/{analysis}".format(project=project, location=location, cluster=cluster, analysis=analysis, )
    actual = LiveVideoAnalyticsClient.analysis_path(project, location, cluster, analysis)
    assert expected == actual


def test_parse_analysis_path():
    expected = {
        "project": "oyster",
        "location": "nudibranch",
        "cluster": "cuttlefish",
        "analysis": "mussel",
    }
    path = LiveVideoAnalyticsClient.analysis_path(**expected)

    # Check that the path construction is reversible.
    actual = LiveVideoAnalyticsClient.parse_analysis_path(path)
    assert expected == actual

def test_cluster_path():
    project = "winkle"
    location = "nautilus"
    cluster = "scallop"
    expected = "projects/{project}/locations/{location}/clusters/{cluster}".format(project=project, location=location, cluster=cluster, )
    actual = LiveVideoAnalyticsClient.cluster_path(project, location, cluster)
    assert expected == actual


def test_parse_cluster_path():
    expected = {
        "project": "abalone",
        "location": "squid",
        "cluster": "clam",
    }
    path = LiveVideoAnalyticsClient.cluster_path(**expected)

    # Check that the path construction is reversible.
    actual = LiveVideoAnalyticsClient.parse_cluster_path(path)
    assert expected == actual

def test_operator_path():
    project = "whelk"
    location = "octopus"
    operator = "oyster"
    expected = "projects/{project}/locations/{location}/operators/{operator}".format(project=project, location=location, operator=operator, )
    actual = LiveVideoAnalyticsClient.operator_path(project, location, operator)
    assert expected == actual


def test_parse_operator_path():
    expected = {
        "project": "nudibranch",
        "location": "cuttlefish",
        "operator": "mussel",
    }
    path = LiveVideoAnalyticsClient.operator_path(**expected)

    # Check that the path construction is reversible.
    actual = LiveVideoAnalyticsClient.parse_operator_path(path)
    assert expected == actual

def test_process_path():
    project = "winkle"
    location = "nautilus"
    cluster = "scallop"
    process = "abalone"
    expected = "projects/{project}/locations/{location}/clusters/{cluster}/processes/{process}".format(project=project, location=location, cluster=cluster, process=process, )
    actual = LiveVideoAnalyticsClient.process_path(project, location, cluster, process)
    assert expected == actual


def test_parse_process_path():
    expected = {
        "project": "squid",
        "location": "clam",
        "cluster": "whelk",
        "process": "octopus",
    }
    path = LiveVideoAnalyticsClient.process_path(**expected)

    # Check that the path construction is reversible.
    actual = LiveVideoAnalyticsClient.parse_process_path(path)
    assert expected == actual

def test_common_billing_account_path():
    billing_account = "oyster"
    expected = "billingAccounts/{billing_account}".format(billing_account=billing_account, )
    actual = LiveVideoAnalyticsClient.common_billing_account_path(billing_account)
    assert expected == actual


def test_parse_common_billing_account_path():
    expected = {
        "billing_account": "nudibranch",
    }
    path = LiveVideoAnalyticsClient.common_billing_account_path(**expected)

    # Check that the path construction is reversible.
    actual = LiveVideoAnalyticsClient.parse_common_billing_account_path(path)
    assert expected == actual

def test_common_folder_path():
    folder = "cuttlefish"
    expected = "folders/{folder}".format(folder=folder, )
    actual = LiveVideoAnalyticsClient.common_folder_path(folder)
    assert expected == actual


def test_parse_common_folder_path():
    expected = {
        "folder": "mussel",
    }
    path = LiveVideoAnalyticsClient.common_folder_path(**expected)

    # Check that the path construction is reversible.
    actual = LiveVideoAnalyticsClient.parse_common_folder_path(path)
    assert expected == actual

def test_common_organization_path():
    organization = "winkle"
    expected = "organizations/{organization}".format(organization=organization, )
    actual = LiveVideoAnalyticsClient.common_organization_path(organization)
    assert expected == actual


def test_parse_common_organization_path():
    expected = {
        "organization": "nautilus",
    }
    path = LiveVideoAnalyticsClient.common_organization_path(**expected)

    # Check that the path construction is reversible.
    actual = LiveVideoAnalyticsClient.parse_common_organization_path(path)
    assert expected == actual

def test_common_project_path():
    project = "scallop"
    expected = "projects/{project}".format(project=project, )
    actual = LiveVideoAnalyticsClient.common_project_path(project)
    assert expected == actual


def test_parse_common_project_path():
    expected = {
        "project": "abalone",
    }
    path = LiveVideoAnalyticsClient.common_project_path(**expected)

    # Check that the path construction is reversible.
    actual = LiveVideoAnalyticsClient.parse_common_project_path(path)
    assert expected == actual

def test_common_location_path():
    project = "squid"
    location = "clam"
    expected = "projects/{project}/locations/{location}".format(project=project, location=location, )
    actual = LiveVideoAnalyticsClient.common_location_path(project, location)
    assert expected == actual


def test_parse_common_location_path():
    expected = {
        "project": "whelk",
        "location": "octopus",
    }
    path = LiveVideoAnalyticsClient.common_location_path(**expected)

    # Check that the path construction is reversible.
    actual = LiveVideoAnalyticsClient.parse_common_location_path(path)
    assert expected == actual


def test_client_with_default_client_info():
    client_info = gapic_v1.client_info.ClientInfo()

    with mock.patch.object(transports.LiveVideoAnalyticsTransport, '_prep_wrapped_messages') as prep:
        client = LiveVideoAnalyticsClient(
            credentials=ga_credentials.AnonymousCredentials(),
            client_info=client_info,
        )
        prep.assert_called_once_with(client_info)

    with mock.patch.object(transports.LiveVideoAnalyticsTransport, '_prep_wrapped_messages') as prep:
        transport_class = LiveVideoAnalyticsClient.get_transport_class()
        transport = transport_class(
            credentials=ga_credentials.AnonymousCredentials(),
            client_info=client_info,
        )
        prep.assert_called_once_with(client_info)

@pytest.mark.asyncio
async def test_transport_close_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport="grpc_asyncio",
    )
    with mock.patch.object(type(getattr(client.transport, "grpc_channel")), "close") as close:
        async with client:
            close.assert_not_called()
        close.assert_called_once()


def test_cancel_operation_rest_bad_request(transport: str = 'rest', request_type=operations_pb2.CancelOperationRequest):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    request = request_type()
    request = json_format.ParseDict({'name': 'projects/sample1/locations/sample2/operations/sample3'}, request)

    # Mock the http request call within the method and fake a BadRequest error.
    with mock.patch.object(Session, 'request') as req, pytest.raises(core_exceptions.BadRequest):
        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 400
        response_value.request = Request()
        req.return_value = response_value
        client.cancel_operation(request)

@pytest.mark.parametrize("request_type", [
    operations_pb2.CancelOperationRequest,
    dict,
])
def test_cancel_operation_rest(request_type):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport="rest",
    )
    request_init = {'name': 'projects/sample1/locations/sample2/operations/sample3'}
    request = request_type(**request_init)
    # Mock the http request call within the method and fake a response.
    with mock.patch.object(type(client.transport._session), 'request') as req:
        # Designate an appropriate value for the returned response.
        return_value = None

        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 200
        json_return_value = '{}'

        response_value._content = json_return_value.encode('UTF-8')
        req.return_value = response_value

        response = client.cancel_operation(request)

    # Establish that the response is the type that we expect.
    assert response is None

def test_delete_operation_rest_bad_request(transport: str = 'rest', request_type=operations_pb2.DeleteOperationRequest):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    request = request_type()
    request = json_format.ParseDict({'name': 'projects/sample1/locations/sample2/operations/sample3'}, request)

    # Mock the http request call within the method and fake a BadRequest error.
    with mock.patch.object(Session, 'request') as req, pytest.raises(core_exceptions.BadRequest):
        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 400
        response_value.request = Request()
        req.return_value = response_value
        client.delete_operation(request)

@pytest.mark.parametrize("request_type", [
    operations_pb2.DeleteOperationRequest,
    dict,
])
def test_delete_operation_rest(request_type):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport="rest",
    )
    request_init = {'name': 'projects/sample1/locations/sample2/operations/sample3'}
    request = request_type(**request_init)
    # Mock the http request call within the method and fake a response.
    with mock.patch.object(type(client.transport._session), 'request') as req:
        # Designate an appropriate value for the returned response.
        return_value = None

        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 200
        json_return_value = '{}'

        response_value._content = json_return_value.encode('UTF-8')
        req.return_value = response_value

        response = client.delete_operation(request)

    # Establish that the response is the type that we expect.
    assert response is None

def test_get_operation_rest_bad_request(transport: str = 'rest', request_type=operations_pb2.GetOperationRequest):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    request = request_type()
    request = json_format.ParseDict({'name': 'projects/sample1/locations/sample2/operations/sample3'}, request)

    # Mock the http request call within the method and fake a BadRequest error.
    with mock.patch.object(Session, 'request') as req, pytest.raises(core_exceptions.BadRequest):
        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 400
        response_value.request = Request()
        req.return_value = response_value
        client.get_operation(request)

@pytest.mark.parametrize("request_type", [
    operations_pb2.GetOperationRequest,
    dict,
])
def test_get_operation_rest(request_type):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport="rest",
    )
    request_init = {'name': 'projects/sample1/locations/sample2/operations/sample3'}
    request = request_type(**request_init)
    # Mock the http request call within the method and fake a response.
    with mock.patch.object(type(client.transport._session), 'request') as req:
        # Designate an appropriate value for the returned response.
        return_value = operations_pb2.Operation()

        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 200
        json_return_value = json_format.MessageToJson(return_value)

        response_value._content = json_return_value.encode('UTF-8')
        req.return_value = response_value

        response = client.get_operation(request)

    # Establish that the response is the type that we expect.
    assert isinstance(response, operations_pb2.Operation)

def test_list_operations_rest_bad_request(transport: str = 'rest', request_type=operations_pb2.ListOperationsRequest):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport=transport,
    )

    request = request_type()
    request = json_format.ParseDict({'name': 'projects/sample1/locations/sample2'}, request)

    # Mock the http request call within the method and fake a BadRequest error.
    with mock.patch.object(Session, 'request') as req, pytest.raises(core_exceptions.BadRequest):
        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 400
        response_value.request = Request()
        req.return_value = response_value
        client.list_operations(request)

@pytest.mark.parametrize("request_type", [
    operations_pb2.ListOperationsRequest,
    dict,
])
def test_list_operations_rest(request_type):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
        transport="rest",
    )
    request_init = {'name': 'projects/sample1/locations/sample2'}
    request = request_type(**request_init)
    # Mock the http request call within the method and fake a response.
    with mock.patch.object(type(client.transport._session), 'request') as req:
        # Designate an appropriate value for the returned response.
        return_value = operations_pb2.ListOperationsResponse()

        # Wrap the value into a proper Response obj
        response_value = Response()
        response_value.status_code = 200
        json_return_value = json_format.MessageToJson(return_value)

        response_value._content = json_return_value.encode('UTF-8')
        req.return_value = response_value

        response = client.list_operations(request)

    # Establish that the response is the type that we expect.
    assert isinstance(response, operations_pb2.ListOperationsResponse)


def test_delete_operation(transport: str = "grpc"):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(), transport=transport,
    )

    # Everything is optional in proto3 as far as the runtime is concerned,
    # and we are mocking out the actual API, so just send an empty request.
    request = operations_pb2.DeleteOperationRequest()

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(type(client.transport.delete_operation), "__call__") as call:
        # Designate an appropriate return value for the call.
        call.return_value = None
        response = client.delete_operation(request)
        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        assert args[0] == request

    # Establish that the response is the type that we expect.
    assert response is None
@pytest.mark.asyncio
async def test_delete_operation_async(transport: str = "grpc"):
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(), transport=transport,
    )

    # Everything is optional in proto3 as far as the runtime is concerned,
    # and we are mocking out the actual API, so just send an empty request.
    request = operations_pb2.DeleteOperationRequest()

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(type(client.transport.delete_operation), "__call__") as call:
        # Designate an appropriate return value for the call.
        call.return_value = grpc_helpers_async.FakeUnaryUnaryCall(
            None
        )
        response = await client.delete_operation(request)
        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        assert args[0] == request

    # Establish that the response is the type that we expect.
    assert response is None

def test_delete_operation_field_headers():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Any value that is part of the HTTP/1.1 URI should be sent as
    # a field header. Set these to a non-empty value.
    request = operations_pb2.DeleteOperationRequest()
    request.name = "locations"

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(type(client.transport.delete_operation), "__call__") as call:
        call.return_value =  None

        client.delete_operation(request)
        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        assert args[0] == request

    # Establish that the field header was sent.
    _, _, kw = call.mock_calls[0]
    assert ("x-goog-request-params", "name=locations",) in kw["metadata"]
@pytest.mark.asyncio
async def test_delete_operation_field_headers_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Any value that is part of the HTTP/1.1 URI should be sent as
    # a field header. Set these to a non-empty value.
    request = operations_pb2.DeleteOperationRequest()
    request.name = "locations"

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(type(client.transport.delete_operation), "__call__") as call:
        call.return_value = grpc_helpers_async.FakeUnaryUnaryCall(
            None
        )
        await client.delete_operation(request)
        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        assert args[0] == request

    # Establish that the field header was sent.
    _, _, kw = call.mock_calls[0]
    assert ("x-goog-request-params", "name=locations",) in kw["metadata"]

def test_delete_operation_from_dict():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )
    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(type(client.transport.delete_operation), "__call__") as call:
        # Designate an appropriate return value for the call.
        call.return_value = None

        response = client.delete_operation(
            request={
                "name": "locations",
            }
        )
        call.assert_called()
@pytest.mark.asyncio
async def test_delete_operation_from_dict_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )
    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(type(client.transport.delete_operation), "__call__") as call:
        # Designate an appropriate return value for the call.
        call.return_value = grpc_helpers_async.FakeUnaryUnaryCall(
            None
        )
        response = await client.delete_operation(
            request={
                "name": "locations",
            }
        )
        call.assert_called()


def test_cancel_operation(transport: str = "grpc"):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(), transport=transport,
    )

    # Everything is optional in proto3 as far as the runtime is concerned,
    # and we are mocking out the actual API, so just send an empty request.
    request = operations_pb2.CancelOperationRequest()

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(type(client.transport.cancel_operation), "__call__") as call:
        # Designate an appropriate return value for the call.
        call.return_value = None
        response = client.cancel_operation(request)
        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        assert args[0] == request

    # Establish that the response is the type that we expect.
    assert response is None
@pytest.mark.asyncio
async def test_cancel_operation_async(transport: str = "grpc"):
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(), transport=transport,
    )

    # Everything is optional in proto3 as far as the runtime is concerned,
    # and we are mocking out the actual API, so just send an empty request.
    request = operations_pb2.CancelOperationRequest()

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(type(client.transport.cancel_operation), "__call__") as call:
        # Designate an appropriate return value for the call.
        call.return_value = grpc_helpers_async.FakeUnaryUnaryCall(
            None
        )
        response = await client.cancel_operation(request)
        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        assert args[0] == request

    # Establish that the response is the type that we expect.
    assert response is None

def test_cancel_operation_field_headers():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Any value that is part of the HTTP/1.1 URI should be sent as
    # a field header. Set these to a non-empty value.
    request = operations_pb2.CancelOperationRequest()
    request.name = "locations"

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(type(client.transport.cancel_operation), "__call__") as call:
        call.return_value =  None

        client.cancel_operation(request)
        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        assert args[0] == request

    # Establish that the field header was sent.
    _, _, kw = call.mock_calls[0]
    assert ("x-goog-request-params", "name=locations",) in kw["metadata"]
@pytest.mark.asyncio
async def test_cancel_operation_field_headers_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Any value that is part of the HTTP/1.1 URI should be sent as
    # a field header. Set these to a non-empty value.
    request = operations_pb2.CancelOperationRequest()
    request.name = "locations"

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(type(client.transport.cancel_operation), "__call__") as call:
        call.return_value = grpc_helpers_async.FakeUnaryUnaryCall(
            None
        )
        await client.cancel_operation(request)
        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        assert args[0] == request

    # Establish that the field header was sent.
    _, _, kw = call.mock_calls[0]
    assert ("x-goog-request-params", "name=locations",) in kw["metadata"]

def test_cancel_operation_from_dict():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )
    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(type(client.transport.cancel_operation), "__call__") as call:
        # Designate an appropriate return value for the call.
        call.return_value = None

        response = client.cancel_operation(
            request={
                "name": "locations",
            }
        )
        call.assert_called()
@pytest.mark.asyncio
async def test_cancel_operation_from_dict_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )
    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(type(client.transport.cancel_operation), "__call__") as call:
        # Designate an appropriate return value for the call.
        call.return_value = grpc_helpers_async.FakeUnaryUnaryCall(
            None
        )
        response = await client.cancel_operation(
            request={
                "name": "locations",
            }
        )
        call.assert_called()


def test_get_operation(transport: str = "grpc"):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(), transport=transport,
    )

    # Everything is optional in proto3 as far as the runtime is concerned,
    # and we are mocking out the actual API, so just send an empty request.
    request = operations_pb2.GetOperationRequest()

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(type(client.transport.get_operation), "__call__") as call:
        # Designate an appropriate return value for the call.
        call.return_value = operations_pb2.Operation()
        response = client.get_operation(request)
        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        assert args[0] == request

    # Establish that the response is the type that we expect.
    assert isinstance(response, operations_pb2.Operation)
@pytest.mark.asyncio
async def test_get_operation_async(transport: str = "grpc"):
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(), transport=transport,
    )

    # Everything is optional in proto3 as far as the runtime is concerned,
    # and we are mocking out the actual API, so just send an empty request.
    request = operations_pb2.GetOperationRequest()

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(type(client.transport.get_operation), "__call__") as call:
        # Designate an appropriate return value for the call.
        call.return_value = grpc_helpers_async.FakeUnaryUnaryCall(
            operations_pb2.Operation()
        )
        response = await client.get_operation(request)
        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        assert args[0] == request

    # Establish that the response is the type that we expect.
    assert isinstance(response, operations_pb2.Operation)

def test_get_operation_field_headers():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Any value that is part of the HTTP/1.1 URI should be sent as
    # a field header. Set these to a non-empty value.
    request = operations_pb2.GetOperationRequest()
    request.name = "locations"

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(type(client.transport.get_operation), "__call__") as call:
        call.return_value = operations_pb2.Operation()

        client.get_operation(request)
        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        assert args[0] == request

    # Establish that the field header was sent.
    _, _, kw = call.mock_calls[0]
    assert ("x-goog-request-params", "name=locations",) in kw["metadata"]
@pytest.mark.asyncio
async def test_get_operation_field_headers_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Any value that is part of the HTTP/1.1 URI should be sent as
    # a field header. Set these to a non-empty value.
    request = operations_pb2.GetOperationRequest()
    request.name = "locations"

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(type(client.transport.get_operation), "__call__") as call:
        call.return_value = grpc_helpers_async.FakeUnaryUnaryCall(
            operations_pb2.Operation()
        )
        await client.get_operation(request)
        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        assert args[0] == request

    # Establish that the field header was sent.
    _, _, kw = call.mock_calls[0]
    assert ("x-goog-request-params", "name=locations",) in kw["metadata"]

def test_get_operation_from_dict():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )
    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(type(client.transport.get_operation), "__call__") as call:
        # Designate an appropriate return value for the call.
        call.return_value = operations_pb2.Operation()

        response = client.get_operation(
            request={
                "name": "locations",
            }
        )
        call.assert_called()
@pytest.mark.asyncio
async def test_get_operation_from_dict_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )
    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(type(client.transport.get_operation), "__call__") as call:
        # Designate an appropriate return value for the call.
        call.return_value = grpc_helpers_async.FakeUnaryUnaryCall(
            operations_pb2.Operation()
        )
        response = await client.get_operation(
            request={
                "name": "locations",
            }
        )
        call.assert_called()


def test_list_operations(transport: str = "grpc"):
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(), transport=transport,
    )

    # Everything is optional in proto3 as far as the runtime is concerned,
    # and we are mocking out the actual API, so just send an empty request.
    request = operations_pb2.ListOperationsRequest()

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(type(client.transport.list_operations), "__call__") as call:
        # Designate an appropriate return value for the call.
        call.return_value = operations_pb2.ListOperationsResponse()
        response = client.list_operations(request)
        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        assert args[0] == request

    # Establish that the response is the type that we expect.
    assert isinstance(response, operations_pb2.ListOperationsResponse)
@pytest.mark.asyncio
async def test_list_operations_async(transport: str = "grpc"):
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(), transport=transport,
    )

    # Everything is optional in proto3 as far as the runtime is concerned,
    # and we are mocking out the actual API, so just send an empty request.
    request = operations_pb2.ListOperationsRequest()

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(type(client.transport.list_operations), "__call__") as call:
        # Designate an appropriate return value for the call.
        call.return_value = grpc_helpers_async.FakeUnaryUnaryCall(
            operations_pb2.ListOperationsResponse()
        )
        response = await client.list_operations(request)
        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        assert args[0] == request

    # Establish that the response is the type that we expect.
    assert isinstance(response, operations_pb2.ListOperationsResponse)

def test_list_operations_field_headers():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Any value that is part of the HTTP/1.1 URI should be sent as
    # a field header. Set these to a non-empty value.
    request = operations_pb2.ListOperationsRequest()
    request.name = "locations"

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(type(client.transport.list_operations), "__call__") as call:
        call.return_value = operations_pb2.ListOperationsResponse()

        client.list_operations(request)
        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        assert args[0] == request

    # Establish that the field header was sent.
    _, _, kw = call.mock_calls[0]
    assert ("x-goog-request-params", "name=locations",) in kw["metadata"]
@pytest.mark.asyncio
async def test_list_operations_field_headers_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )

    # Any value that is part of the HTTP/1.1 URI should be sent as
    # a field header. Set these to a non-empty value.
    request = operations_pb2.ListOperationsRequest()
    request.name = "locations"

    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(type(client.transport.list_operations), "__call__") as call:
        call.return_value = grpc_helpers_async.FakeUnaryUnaryCall(
            operations_pb2.ListOperationsResponse()
        )
        await client.list_operations(request)
        # Establish that the underlying gRPC stub method was called.
        assert len(call.mock_calls) == 1
        _, args, _ = call.mock_calls[0]
        assert args[0] == request

    # Establish that the field header was sent.
    _, _, kw = call.mock_calls[0]
    assert ("x-goog-request-params", "name=locations",) in kw["metadata"]

def test_list_operations_from_dict():
    client = LiveVideoAnalyticsClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )
    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(type(client.transport.list_operations), "__call__") as call:
        # Designate an appropriate return value for the call.
        call.return_value = operations_pb2.ListOperationsResponse()

        response = client.list_operations(
            request={
                "name": "locations",
            }
        )
        call.assert_called()
@pytest.mark.asyncio
async def test_list_operations_from_dict_async():
    client = LiveVideoAnalyticsAsyncClient(
        credentials=ga_credentials.AnonymousCredentials(),
    )
    # Mock the actual call within the gRPC stub, and fake the request.
    with mock.patch.object(type(client.transport.list_operations), "__call__") as call:
        # Designate an appropriate return value for the call.
        call.return_value = grpc_helpers_async.FakeUnaryUnaryCall(
            operations_pb2.ListOperationsResponse()
        )
        response = await client.list_operations(
            request={
                "name": "locations",
            }
        )
        call.assert_called()


def test_transport_close():
    transports = {
        "rest": "_session",
        "grpc": "_grpc_channel",
    }

    for transport, close_name in transports.items():
        client = LiveVideoAnalyticsClient(
            credentials=ga_credentials.AnonymousCredentials(),
            transport=transport
        )
        with mock.patch.object(type(getattr(client.transport, close_name)), "close") as close:
            with client:
                close.assert_not_called()
            close.assert_called_once()

def test_client_ctx():
    transports = [
        'rest',
        'grpc',
    ]
    for transport in transports:
        client = LiveVideoAnalyticsClient(
            credentials=ga_credentials.AnonymousCredentials(),
            transport=transport
        )
        # Test client calls underlying transport.
        with mock.patch.object(type(client.transport), "close") as close:
            close.assert_not_called()
            with client:
                pass
            close.assert_called()

@pytest.mark.parametrize("client_class,transport_class", [
    (LiveVideoAnalyticsClient, transports.LiveVideoAnalyticsGrpcTransport),
    (LiveVideoAnalyticsAsyncClient, transports.LiveVideoAnalyticsGrpcAsyncIOTransport),
])
def test_api_key_credentials(client_class, transport_class):
    with mock.patch.object(
        google.auth._default, "get_api_key_credentials", create=True
    ) as get_api_key_credentials:
        mock_cred = mock.Mock()
        get_api_key_credentials.return_value = mock_cred
        options = client_options.ClientOptions()
        options.api_key = "api_key"
        with mock.patch.object(transport_class, "__init__") as patched:
            patched.return_value = None
            client = client_class(client_options=options)
            patched.assert_called_once_with(
                credentials=mock_cred,
                credentials_file=None,
                host=client.DEFAULT_ENDPOINT,
                scopes=None,
                client_cert_source_for_mtls=None,
                quota_project_id=None,
                client_info=transports.base.DEFAULT_CLIENT_INFO,
                always_use_jwt_access=True,
                api_audience=None,
            )
