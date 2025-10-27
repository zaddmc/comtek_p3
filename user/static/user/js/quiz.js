document.addEventListener('DOMContentLoaded', function() {
    // Modal elements
    const modal = document.getElementById('quizModal');
    const startQuizBtn = document.getElementById('startQuiz');
    const browseSuitcasesBtn = document.getElementById('browseSuitcases');
    const closeBtn = document.querySelector('.close');
    const suitcasesList = document.getElementById('suitcasesList');
    const welcomeSection = document.querySelector('.welcome-section');
    const notuser = document.querySelector('.notuser');

    // Quiz elements
    const quizSteps = document.querySelectorAll('.quiz-step');
    const prevBtn = document.getElementById('prevBtn');
    const nextBtn = document.getElementById('nextBtn');
    const submitBtn = document.getElementById('submitQuiz');
    const stepIndicator = document.getElementById('stepIndicator');
    const optionBtns = document.querySelectorAll('.option-btn');

    let currentStep = 0;
    const userAnswers = {};

    // Modal handlers
    startQuizBtn.addEventListener('click', () => {
        modal.style.display = 'block';
        // Hide the main content when modal opens
        notuser.style.display = 'none';
        resetQuiz();
    });

    browseSuitcasesBtn.addEventListener('click', () => {
        welcomeSection.style.display = 'none';
        suitcasesList.style.display = 'block';
    });

    closeBtn.addEventListener('click', () => {
        modal.style.display = 'none';
        // Show the main content again when modal closes
        notuser.style.display = 'flex';
    });

    window.addEventListener('click', (event) => {
        if (event.target === modal) {
            modal.style.display = 'none';
            // Show the main content again when clicking outside modal
            notuser.style.display = 'flex';
        }
    });

    // Quiz navigation
    prevBtn.addEventListener('click', () => {
        if (currentStep > 0) {
            currentStep--;
            updateStep();
        }
    });

    nextBtn.addEventListener('click', () => {
        // Check if current question is answered before proceeding
        const currentStepId = quizSteps[currentStep].id;
        if (!userAnswers[currentStepId]) {
            alert('Please select an answer before continuing.');
            return;
        }
        
        if (currentStep < quizSteps.length - 1) {
            currentStep++;
            updateStep();
        }
    });

    submitBtn.addEventListener('click', () => {
        // Check if final question is answered before submitting
        const currentStepId = quizSteps[currentStep].id;
        if (!userAnswers[currentStepId]) {
            alert('Please select an answer before submitting.');
            return;
        }
        submitQuiz();
    });

    // Option selection
    optionBtns.forEach(btn => {
        btn.addEventListener('click', function() {
            // Remove selected class from siblings
            const siblings = this.parentElement.querySelectorAll('.option-btn');
            siblings.forEach(sibling => sibling.classList.remove('selected'));
            
            // Add selected class to clicked button
            this.classList.add('selected');
            
            // Store the answer
            const question = this.closest('.quiz-step').id;
            userAnswers[question] = this.dataset.value;
            
            // Enable next button if it was disabled due to validation
            updateButtonStates();
        });
    });

    function updateStep() {
        // Hide all steps
        quizSteps.forEach(step => step.classList.remove('active'));
        
        // Show current step
        quizSteps[currentStep].classList.add('active');
        
        // Update step indicator
        stepIndicator.textContent = `Question ${currentStep + 1} of ${quizSteps.length}`;
        
        updateButtonStates();
    }

    function updateButtonStates() {
        const currentStepId = quizSteps[currentStep].id;
        const isAnswered = !!userAnswers[currentStepId];
        
        // Update button states
        prevBtn.disabled = currentStep === 0;
        
        if (currentStep === quizSteps.length - 1) {
            nextBtn.style.display = 'none';
            submitBtn.style.display = 'inline-block';
            // Enable submit button only if current question is answered
            submitBtn.disabled = !isAnswered;
        } else {
            nextBtn.style.display = 'inline-block';
            submitBtn.style.display = 'none';
            // Enable next button only if current question is answered
            nextBtn.disabled = !isAnswered;
        }
    }

    function resetQuiz() {
        currentStep = 0;
        Object.keys(userAnswers).forEach(key => delete userAnswers[key]);
        
        // Clear selections
        optionBtns.forEach(btn => btn.classList.remove('selected'));
        
        updateStep();
    }

    function submitQuiz() {
        // Simply show the suitcases list after quiz completion
        modal.style.display = 'none';
        // Show main content again after quiz completion
        notuser.style.display = 'flex';
        welcomeSection.style.display = 'none';
        suitcasesList.style.display = 'block';
        
        // Log answers for potential future use
        console.log('User answers:', userAnswers);
    }
});