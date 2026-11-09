let speed = Math.round($prop('SpeedKmh') || 0);
let rpm = Math.round($prop('Rpms') || 0);
let rpmPercent = Math.round(($prop('DataCorePlugin.GameData.CarSettings_CurrentDisplayedRPMPercent') || 0) / 3.3);
let abs = ($prop('GameRawData.Physics.Abs') > 0) ? 1 : 0;
let lap = String($prop('CurrentLapTime') || "00:00:00.0").substring(3, 10) || "00:00.0";
let gear = $prop('Gear') || 'N';
return `${speed};${rpm};${rpmPercent};${abs};${lap};${gear}\n`;
