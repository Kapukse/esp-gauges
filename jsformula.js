let speed = Math.round($prop('SpeedKmh') || 0);
let rpm = Math.round($prop('Rpms') || 0);
let rpmPercent = Math.round(($prop('GameData.CarSettings_CurrentDisplayedRPMPercent') || 0) / 3.3);
let abs = ($prop('GameRawData.Physics.ABS'));
let lapRaw = $prop('CurrentLapTime');
let lap = 0;
if (lapRaw) {
    lap = Math.floor(lapRaw.TotalSeconds || 0);
}

let gear = $prop('Gear') || 'N';

return `${speed};${rpm};${rpmPercent};${abs};${lap};${gear}\n`;