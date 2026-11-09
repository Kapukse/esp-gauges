let speed = Math.round($prop('SpeedKmh') || 0);
let rpm = Math.round($prop('Rpms') || 0);
let rpmPercent = Math.round(($prop('GameData.CarSettings_CurrentDisplayedRPMPercent') || 0) / 3.3);
let abs = ($prop('GameData.Physics.ABS'));
let lap = lapTime ? Number(lapTime.TotalSeconds || 0).toFixed(2) : "0.00";
let gear = $prop('Gear') || 'N';

return `${speed};${rpm};${rpmPercent};${abs};${lap};${gear}\n`;