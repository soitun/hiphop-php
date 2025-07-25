/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package e2e

import (
	"context"
	"net"
	"testing"
	"time"

	"github.com/facebook/fbthrift/thrift/lib/go/thrift"
	"thrift/lib/go/thrift/e2e/service"

	"github.com/stretchr/testify/require"
)

// TestServiceHeaders ensures that persistent headers can be retrieved
// from the context passed to the server handler.
func TestServiceHeaders(t *testing.T) {
	t.Run("Header", func(t *testing.T) {
		runHeaderTest(t, thrift.TransportIDHeader)
	})
	t.Run("UpgradeToRocket", func(t *testing.T) {
		runHeaderTest(t, thrift.TransportIDUpgradeToRocket)
	})
	t.Run("Rocket", func(t *testing.T) {
		runHeaderTest(t, thrift.TransportIDRocket)
	})
}

func runHeaderTest(t *testing.T, serverTransport thrift.TransportID) {
	const (
		persistentHeaderKey   = "persistentHeader"
		persistentHeaderValue = "persistentHeaderValue"
		rpcHeaderKey          = "rpcHeader"
		rpcHeaderValue        = "rpcHeaderValue"
	)

	var clientTransportOption thrift.ClientOption
	switch serverTransport {
	case thrift.TransportIDHeader:
		clientTransportOption = thrift.WithHeader()
	case thrift.TransportIDUpgradeToRocket:
		clientTransportOption = thrift.WithUpgradeToRocket()
	case thrift.TransportIDRocket:
		clientTransportOption = thrift.WithRocket()
	}

	addr, stopServer := startE2EServer(t, serverTransport, thrift.WithNumWorkers(10))
	defer stopServer()

	channel, err := thrift.NewClient(
		clientTransportOption,
		thrift.WithDialer(func() (net.Conn, error) {
			return net.DialTimeout("unix", addr.String(), 5*time.Second)
		}),
		thrift.WithIoTimeout(5*time.Second),
		thrift.WithPersistentHeader(persistentHeaderKey, persistentHeaderValue),
	)
	require.NoError(t, err)
	client := service.NewE2EChannelClient(channel)
	defer client.Close()
	ctx := thrift.WithHeaders(context.Background(), map[string]string{rpcHeaderKey: rpcHeaderValue})
	echoedHeaders, err := client.EchoHeaders(ctx)
	require.NoError(t, err)
	require.Equal(t, echoedHeaders[persistentHeaderKey], persistentHeaderValue)
	require.Equal(t, echoedHeaders[rpcHeaderKey], rpcHeaderValue)
}
