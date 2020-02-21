﻿/*
 * Copyright 2019, Digi International Inc.
 * Copyright 2014, 2015, Sébastien Rault.
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

using System.Collections.Generic;

namespace XBeeLibrary.Core.Models
{
	/// <summary>
	/// Enumerates the different options used in the discovery process.
	/// </summary>
	public enum DiscoveryOptions : byte
	{
		// Enumeration entries.
		/// <summary>
		/// Append device type identifier (DD) to the discovery response.
		/// </summary>
		/// <remarks>Valid for the following protocols:
		/// <list type="bullet">
		/// <item><description>DigiMesh</description></item>
		/// <item><description>Point-to-multipoint (Digi Point)</description></item>
		/// <item><description>ZigBee</description></item></list>
		/// </remarks>
		APPEND_DD = 1,

		/// <summary>
		/// Local device sends response frame when discovery is issued.
		/// </summary>
		/// <remarks>Valid for the following protocols:
		/// <list type="bullet">
		/// <item><description>DigiMesh</description></item>
		/// <item><description>Point-to-multipoint (Digi Point)</description></item>
		/// <item><description>ZigBee</description></item>
		/// <item><description>802.15.4</description></item></list>
		/// </remarks>
		DISCOVER_MYSELF = 2,

		/// <summary>
		/// Append RSSI of the last hop to the discovery response.
		/// </summary>
		/// <remarks>Valid for the following protocols:
		/// <list type="bullet">
		/// <item><description>DigiMesh</description></item>
		/// <item><description>Point-to-multipoint (Digi Point)</description></item></list>
		/// </remarks>
		APPEND_RSSI = 4
	}

	public static class DiscoveryOptionsExtensions
	{
		static IDictionary<DiscoveryOptions, string> lookupTable = new Dictionary<DiscoveryOptions, string>();

		static DiscoveryOptionsExtensions()
		{
			lookupTable.Add(DiscoveryOptions.APPEND_DD, "Append device type identifier (DD)");
			lookupTable.Add(DiscoveryOptions.DISCOVER_MYSELF, "Local device sends response frame");
			lookupTable.Add(DiscoveryOptions.APPEND_RSSI, "Append RSSI (of the last hop)");
		}

		/// <summary>
		/// Gets the value of the discovery option.
		/// </summary>
		/// <param name="source"></param>
		/// <returns>The value of the discovery option.</returns>
		public static int GetValue(this DiscoveryOptions source)
		{
			return (int)source;
		}

		/// <summary>
		/// Gets the description of the discovery option.
		/// </summary>
		/// <param name="source"></param>
		/// <returns>The discovery option description.</returns>
		public static string GetDescription(this DiscoveryOptions source)
		{
			return lookupTable[source];
		}

		/// <summary>
		/// Calculates the total value of a combination of several options for the given protocol.
		/// </summary>
		/// <param name="source"></param>
		/// <param name="protocol">The <see cref="XBeeProtocol"/> to calculate the value of all the given 
		/// discovery options.</param>
		/// <param name="options">Collection of options to get the final value.</param>
		/// <returns>The value to be configured in the module depending on the given collection of options 
		/// and the protocol.</returns>
		/// <seealso cref="XBeeProtocol"/>
		public static int CalculateDiscoveryValue(this DiscoveryOptions source, XBeeProtocol protocol, ISet<DiscoveryOptions> options)
		{
			// Calculate value to be configured.
			int value = 0;
			switch (protocol)
			{
				case XBeeProtocol.ZIGBEE:
				case XBeeProtocol.ZNET:
					foreach (DiscoveryOptions op in options)
					{
						if (op == DiscoveryOptions.APPEND_RSSI)
							continue;
						value = value + op.GetValue();
					}
					break;
				case XBeeProtocol.DIGI_MESH:
				case XBeeProtocol.DIGI_POINT:
				case XBeeProtocol.XLR:
				// TODO [XLR_DM] The next version of the XLR will add DigiMesh support.
				// For the moment only point-to-multipoint is supported in this kind of devices.
				case XBeeProtocol.XLR_DM:
					foreach (DiscoveryOptions op in options)
						value = value + op.GetValue();
					break;
				case XBeeProtocol.RAW_802_15_4:
				case XBeeProtocol.UNKNOWN:
				default:
					if (options.Contains(DiscoveryOptions.DISCOVER_MYSELF))
						value = 1; // This is different for 802.15.4.
					break;
			}
			return value;
		}

		/// <summary>
		/// Returns the <see cref="DiscoveryOptions"/> object in string format.
		/// </summary>
		/// <param name="source"></param>
		/// <returns>The <see cref="DiscoveryOptions"/> object in string format.</returns>
		public static string ToDisplayString(this DiscoveryOptions source)
		{
			var value = lookupTable[source];

			return string.Format("{0} ({1})", value, (byte)source);
		}
	}
}