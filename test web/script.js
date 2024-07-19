// script.js

document.addEventListener('DOMContentLoaded', function () {
    var ctx = document.getElementById('powerChart').getContext('2d');
    var powerChart = new Chart(ctx, {
        type: 'line',
        data: {
            labels: ['0h', '2h', '4h', '6h', '8h', '10h', '12h', '14h', '16h', '18h', '20h', '22h'],
            datasets: [
                {
                    label: 'Generación FV',
                    borderColor: '#7CE6AB',
                    data: [0, 0, 0, 5, 20, 40, 60, 80, 60, 40, 20, 0],
                    fill: true,
                    backgroundColor: createGradient(ctx, '#7CE6AB'),
                    tension: 0.4
                },
                {
                    label: 'Consumo',
                    borderColor: '#FF808B',
                    data: [10, 15, 20, 25, 30, 35, 40, 45, 40, 35, 30, 25],
                    fill: true,
                    backgroundColor: createGradient(ctx, '#FF808B'),
                    tension: 0.4
                },
                {
                    label: 'Autoconsumo',
                    borderColor: '#5E81F4',
                    data: [5, 10, 15, 20, 25, 30, 35, 40, 35, 30, 25, 20],
                    fill: true,
                    backgroundColor: createGradient(ctx, '#5E81F4'),
                    tension: 0.4
                }
            ]
        },
        options: {
            responsive: true,
            maintainAspectRatio: false,
            scales: {
                x: {
                    display: true,
                    title: {
                        display: true,
                        text: 'Hora'
                    }
                },
                y: {
                    display: true,
                    title: {
                        display: true,
                        text: 'Potencia (kW)'
                    },
                    suggestedMin: 0,
                    suggestedMax: 100
                }
            }
        }
    });

    function createGradient(ctx, color) {
        var gradient = ctx.createLinearGradient(0, 0, 0, 200);
        gradient.addColorStop(0, color + '99');
        gradient.addColorStop(1, color + '00');
        return gradient;
    }
});
